#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>
#include <queue>

namespace
{

  struct GraphNode
  {
    GraphNode() = default;
    GraphNode(std::vector<size_t> connections) : connections(connections) {}

    bool operator==(const GraphNode &other) const
    {
      return this->connections == other.connections;
    }

    friend std::ostream &operator<<(std::ostream &out, GraphNode const &node)
    {
      for (const auto &connection : node.connections)
      {
        out << connection << " ";
      }
      return out;
    }

    void AddConnection(size_t nodeNumber)
    {
      connections.push_back(nodeNumber);
      std::sort(connections.begin(), connections.end());
    }

    std::vector<size_t> connections;
  };

  using GraphNodes = std::vector<GraphNode>;

  struct NodeInfo
  {
    bool wasChecked{false};
    int distanceFromStartPoint{0};
  };

  class Map
  {
  public:
    Map(std::string points, size_t rowSize) : points(points), rowSize(rowSize)
    {
      startingPoint = points.find("S");
      endingPoint = points.find("E");
      if (startingPoint == std::string::npos || endingPoint == std::string::npos)
      {
        startingPoint = 0;
        endingPoint = points.size() - 1;
      }
      else
      {
        this->points.at(startingPoint) = 'a';
        this->points.at(endingPoint) = 'z';
      }
    }

    std::vector<size_t> GetLowestPoints()
    {
      std::vector<size_t> lowestPoints{};
      std::string lowestValue{"a"};
      size_t index = 0;
      while ((index = points.find(lowestValue, index)) != std::string::npos)
      {
        lowestPoints.push_back(index);
        index += lowestValue.size();
      }
      return lowestPoints;
    }

    bool IsEnvelopAchivable(char current, char next)
    {
      return static_cast<int>(next) - static_cast<int>(current) < 2;
    }

    GraphNodes ConvertToGraph()
    {
      const size_t rowsCount = points.size() / rowSize;
      GraphNodes nodes;

      for (size_t i = 0; i < points.size(); ++i)
      {
        size_t x = i % rowSize;
        size_t y = i / rowSize;

        GraphNode node;

        if (size_t previousPointInRow = i - 1;
            x > 0 && IsEnvelopAchivable(points.at(i), points.at(previousPointInRow)))
        {
          node.AddConnection(previousPointInRow);
        }
        if (size_t nextPointInRow = i + 1;
            x + 1 < rowSize && IsEnvelopAchivable(points.at(i), points.at(nextPointInRow)))
        {
          node.AddConnection(nextPointInRow);
        }
        if (size_t previuosPointInColumn = i - rowSize;
            y > 0 && IsEnvelopAchivable(points.at(i), points.at(previuosPointInColumn)))
        {
          node.AddConnection(i - rowSize);
        }
        if (size_t nextPointInColumn = i + rowSize;
            y + 1 < rowsCount && IsEnvelopAchivable(points.at(i), points.at(nextPointInColumn)))
        {
          node.AddConnection(i + rowSize);
        }
        nodes.push_back(node);
      }

      return nodes;
    }

    size_t GetStartingPoint()
    {
      return startingPoint;
    }

    size_t GetEndingPoint()
    {
      return endingPoint;
    }

  private:
    std::string points;
    size_t rowSize;
    size_t startingPoint;
    size_t endingPoint;
  };

  int BFS(const GraphNodes &nodes, size_t startNodeNumber, size_t endNodeNumber)
  {
    std::vector<NodeInfo> nodesInfo(nodes.size());
    std::queue<size_t> nodesQueue;
    nodesQueue.push(startNodeNumber);

    while (!nodesQueue.empty())
    {
      const size_t currNodeNumber = nodesQueue.front();
      nodesInfo[currNodeNumber].wasChecked = true;
      nodesQueue.pop();

      if (currNodeNumber == endNodeNumber)
        return nodesInfo[currNodeNumber].distanceFromStartPoint;

      for (size_t connectionNodeNumber : nodes[currNodeNumber].connections)
      {
        if (!nodesInfo[connectionNodeNumber].wasChecked)
        {
          nodesQueue.push(connectionNodeNumber);
          nodesInfo[connectionNodeNumber].wasChecked = true;
          nodesInfo[connectionNodeNumber].distanceFromStartPoint = nodesInfo[currNodeNumber].distanceFromStartPoint + 1;
        }
      }
    }
    return -1;
  }

  Map ReadMapPoints(std::basic_istream<char> &input)
  {
    std::string points{};
    size_t rowSize{};

    std::string segment;
    while (std::getline(input, segment))
    {
      points += segment;
      rowSize = segment.size();
    }

    return Map{points, rowSize};
  }

}

TEST_CASE("Create Graph")
{
  Map map{"abbdcd", 2};
  GraphNodes expectesGraphNodes{
      {{1, 2}},
      {{0}},
      {{0, 4}},
      {{1, 2, 5}},
      {{2, 5}},
      {{3, 4}}};

  CHECK(expectesGraphNodes == map.ConvertToGraph());
  CHECK(3 == BFS(map.ConvertToGraph(), map.GetStartingPoint(), map.GetEndingPoint()));
}

TEST_CASE("example data")
{
  std::stringstream test{};

  test << "Sabqponm\n"
       << "abcryxxl\n"
       << "accszExk\n"
       << "acctuvwj\n"
       << "abdefghi\n";

  auto map = ReadMapPoints(test);

  SECTION("task 1")
  {
    CHECK(31 == BFS(map.ConvertToGraph(), map.GetStartingPoint(), map.GetEndingPoint()));
  }

  SECTION("task 2")
  {
    std::vector<int> pathsLengths;

    const auto lowestPoints = map.GetLowestPoints();
    CHECK(6 == lowestPoints.size());
    for (size_t i = 0; i < lowestPoints.size(); i++)
    {
      pathsLengths.push_back(BFS(map.ConvertToGraph(), lowestPoints.at(i), map.GetEndingPoint()));
    }

    CHECK(29 == *std::min_element(pathsLengths.begin(), pathsLengths.end()));
  }
}

TEST_CASE("read day 12 data")
{
  std::fstream my_file;
  my_file.open("day12_data.txt", std::ios::in);

  auto map = ReadMapPoints(my_file);

  SECTION("task 1")
  {
    std::cout << "Day 12 task 1 result: " << BFS(map.ConvertToGraph(), map.GetStartingPoint(), map.GetEndingPoint()) << std::endl;
  }

  SECTION("task 2")
  {
    std::vector<int> pathsLengths;

    const auto lowestPoints = map.GetLowestPoints();
    for (size_t i = 0; i < lowestPoints.size(); i++)
    {
      const auto pathLength = BFS(map.ConvertToGraph(), lowestPoints.at(i), map.GetEndingPoint());
      if (pathLength > 0)
      {
        pathsLengths.push_back(pathLength);
      }
    }

    std::cout << "Day 12 task 2 result: " << *std::min_element(pathsLengths.begin(), pathsLengths.end()) << std::endl;
  }
}
