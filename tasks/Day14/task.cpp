#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>
#include <unordered_map>

namespace
{
  struct Point
  {
    int x;
    int y;

    friend std::istream &operator>>(std::istream &is, Point &p)
    {
      char delim;
      is >> p.x >> delim >> p.y;
      return is;
    }

    bool operator==(const Point &other) const
    {
      return this->x == other.x && this->y == other.y;
    }
  };

  struct PointHash
  {
    size_t operator()(const Point &p) const
    {
      return std::hash<int>{}(p.x) ^ std::hash<int>{}(p.y);
    }
  };

  class Cave
  {
  public:
    Cave(std::istream &is)
    {
      std::string scan;
      while (std::getline(is, scan))
      {
        addScan(scan);
      }
      caveRocks = static_cast<int>(cavePoints.size());

      leftEdge = std::min_element(cavePoints.begin(), cavePoints.end(), [](const auto &p, const auto &smallest)
                                  { return p.first.x < smallest.first.x; })
                     ->first.x;
      rightEdge = std::max_element(cavePoints.begin(), cavePoints.end(), [](const auto &p, const auto &smallest)
                                   { return p.first.x < smallest.first.x; })
                      ->first.x;
      bottomEdge = std::max_element(cavePoints.begin(), cavePoints.end(), [](const auto &p, const auto &smallest)
                                    { return p.first.y < smallest.first.y; })
                       ->first.y;
    };

    virtual ~Cave() = default;

    void addScan(const std::string &scan)
    {
      std::stringstream ss{scan};
      std::vector<Point> points;
      while (ss)
      {
        char arrow;
        Point p;
        ss >> p;
        points.push_back(p);
        ss >> arrow >> arrow;
      }
      for (size_t i = 1; i < points.size(); ++i)
      {
        const auto &first = points.at(i - 1);
        const auto &second = points.at(i);
        if (first.x == second.x)
        {
          for (int y = std::min(first.y, second.y); y <= std::max(first.y, second.y); ++y)
          {
            cavePoints.emplace(std::make_pair<Point, char>({first.x, y}, '#'));
          }
        }
        if (first.y == second.y)
        {
          for (int x = std::min(first.x, second.x); x <= std::max(first.x, second.x); ++x)
          {
            cavePoints.emplace(std::make_pair<Point, char>(Point{x, first.y}, '#'));
          }
        }
      }
    }

    int GetCaveRocksCount() const
    {
      return caveRocks;
    }

    int GetSandCount() const
    {
      return static_cast<int>(cavePoints.size()) - caveRocks;
    }

    virtual void DropSand(Point sand = {500, 0})
    {
      while (IsInBondaries(sand))
      {
        if (!IsObstacle(sand.x, sand.y + 1))
        {
          ++sand.y;
        }
        else if (!IsObstacle(sand.x - 1, sand.y + 1))
        {
          ++sand.y;
          --sand.x;
        }
        else if (!IsObstacle(sand.x + 1, sand.y + 1))
        {
          ++sand.y;
          ++sand.x;
        }
        else
        {
          cavePoints.emplace(std::make_pair<Point, char>(Point{sand.x, sand.y}, 'o'));
          return;
        }
      }
    }

    virtual bool IsObstacle(int x, int y)
    {
      return cavePoints.find(Point{x, y}) != cavePoints.end();
    }

    virtual bool IsInBondaries(const Point &p)
    {
      return p.x >= this->leftEdge &&
             p.x <= this->rightEdge &&
             p.y <= this->bottomEdge;
    }

    friend std::ostream &operator<<(std::ostream &os, const Cave &c)
    {
      for (int y = c.topEdge; y <= c.bottomEdge; ++y)
      {
        for (int x = c.leftEdge; x <= c.rightEdge; ++x)
        {
          if (const auto &p = c.cavePoints.find(Point{x, y});
              p != c.cavePoints.end())
          {
            os << p->second;
          }
          else
          {
            os << '.';
          }
        }
        os << std::endl;
      }
      return os;
    }

  protected:
    int caveRocks;
    std::unordered_map<Point, char, PointHash> cavePoints;

    int topEdge = 0;
    int leftEdge = std::numeric_limits<int>::max();
    int rightEdge = std::numeric_limits<int>::min();
    int bottomEdge = std::numeric_limits<int>::min();
  };

  class FiniteCave : public Cave
  {
  public:
    FiniteCave(std::istream &is) : Cave(is)
    {
      bottomEdge += 2;
    }

    ~FiniteCave() override = default;

    bool IsObstacle(int x, int y) override
    {
      return Cave::IsObstacle(x, y) || y == bottomEdge;
    }

    bool IsInBondaries(const Point &) override
    {
      return true;
    }

    void DropSand(Point sand = {500, 0}) override
    {
      if (cavePoints.find(sand) != cavePoints.end())
      {
        return;
      }

      Cave::DropSand();

      leftEdge = std::min_element(cavePoints.begin(), cavePoints.end(), [](const auto &p, const auto &smallest)
                                  { return p.first.x < smallest.first.x; })
                     ->first.x;
      rightEdge = std::max_element(cavePoints.begin(), cavePoints.end(), [](const auto &p, const auto &smallest)
                                   { return p.first.x < smallest.first.x; })
                      ->first.x;
    }
  };

}

TEST_CASE("Read points")
{
  std::stringstream ss;

  SECTION("vertical line")
  {
    ss << "100,1 -> 100,3";
    Cave c{ss};
    CHECK(c.GetCaveRocksCount() == 3);
  }

  SECTION("horizontal line")
  {
    ss << "100,1 -> 102,1";
    Cave c{ss};
    CHECK(c.GetCaveRocksCount() == 3);
  }

  SECTION("break line")
  {
    ss << "100,1 -> 102,1 -> 102,3";
    Cave c{ss};
    CHECK(c.GetCaveRocksCount() == 5);
  }
}

void DropSandUnits(Cave &c, int units)
{
  for (int i = 0; i < units; ++i)
  {
    c.DropSand();
  }
}

TEST_CASE("example data")
{
  std::stringstream ss;

  ss << "498,4 -> 498,6 -> 496,6\n"
     << "503,4 -> 502,4 -> 502,9 -> 494,9";

  SECTION("infinite cave")
  {
    Cave c{ss};
    CHECK(c.GetSandCount() == 0);

    DropSandUnits(c, 30);
    CHECK(c.GetSandCount() == 24);

    std::cout << c;
  }

  SECTION("finite cave")
  {
    FiniteCave c{ss};
    CHECK(c.GetSandCount() == 0);

    DropSandUnits(c, 100);
    CHECK(c.GetSandCount() == 93);
  }
}

TEST_CASE("read day 14 data")
{
  std::fstream my_file;
  my_file.open("day14_data.txt", std::ios::in);

  SECTION("task 1")
  {
    Cave c{my_file};
    int isFull = false;

    while (!isFull)
    {
      int lastSandCount = c.GetSandCount();
      c.DropSand();
      isFull = lastSandCount == c.GetSandCount();
    }

    std::cout << "Day 14 task 1 result: " << c.GetSandCount() << std::endl;
  }

  SECTION("task 2")
  {
    FiniteCave c{my_file};

    int isFull = false;

    while (!isFull)
    {
      int lastSandCount = c.GetSandCount();
      c.DropSand();
      isFull = lastSandCount == c.GetSandCount();
    }

    std::cout << "Day 14 task 2 result: " << c.GetSandCount() << std::endl;
  }
}