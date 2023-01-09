#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{
  using Knot = std::pair<int, int>;

  class Rope
  {
  public:
    Rope(size_t knotCount = 2) : knots(knotCount)
    {
    }

    Knot GetHead() const
    {
      return knots.front();
    }

    Knot GetTail() const
    {
      return knots.back();
    }

    void MoveHead(const std::string &direction)
    {
      MoveKnotWithDirection(knots.front(), direction);

      for (size_t i = 1; i < knots.size(); ++i)
      {
        AdjustTailKnotPosition(i);
      }
    }

  private:
    void MoveKnotWithDirection(Knot &knot, std::string direction)
    {
      if (direction == MoveUp)
      {
        ++knot.second;
      }
      else if (direction == MoveDown)
      {
        --knot.second;
      }
      else if (direction == MoveRight)
      {
        ++knot.first;
      }
      else if (direction == MoveLeft)
      {
        --knot.first;
      }
    }

    void AdjustTailKnotPosition(size_t tailKnotNo)
    {
      Knot &head = knots.at(tailKnotNo - 1);
      Knot &tail = knots.at(tailKnotNo);

      const int verticalDistance = head.second - tail.second;
      const int horizontalDistance = head.first - tail.first;

      if (AreInTheSameRowOrColumn(head, tail) && std::abs(verticalDistance) > 1)
      {
        MoveKnotWithDirection(tail, verticalDistance > 0 ? MoveUp : MoveDown);
      }
      else if (AreInTheSameRowOrColumn(head, tail) && std::abs(horizontalDistance) > 1)
      {
        MoveKnotWithDirection(tail, horizontalDistance > 0 ? MoveRight : MoveLeft);
      }
      else if (ShouldBeMovedDiagonally(head, tail))
      {
        MoveKnotWithDirection(tail, verticalDistance > 0 ? MoveUp : MoveDown);
        MoveKnotWithDirection(tail, horizontalDistance > 0 ? MoveRight : MoveLeft);
      }
    }

    bool AreInTheSameRowOrColumn(const Knot &head, const Knot &tail)
    {
      return head.first == tail.first || head.second == tail.second;
    }

    bool ShouldBeMovedDiagonally(const Knot &head, const Knot &tail)
    {
      const int verticalDistance = head.second - tail.second;
      const int horizontalDistance = head.first - tail.first;

      return std::abs(verticalDistance) > 1 || std::abs(horizontalDistance) > 1;
    }

    const std::string MoveUp = "U";
    const std::string MoveDown = "D";
    const std::string MoveRight = "R";
    const std::string MoveLeft = "L";

    std::vector<Knot> knots;
  };

  std::pair<std::string, int> GetDirectionAndValue(const std::string &instruction)
  {
    const auto spacePosition = instruction.find(" ");
    return {instruction.substr(0, spacePosition), std::stoi(instruction.substr(spacePosition, instruction.size()))};
  }

  std::set<Knot> GetTailPositions(std::basic_istream<char> &instructions, size_t knotsCount = 2)
  {
    Rope rope{knotsCount};
    std::set<Knot> tailPositions;
    tailPositions.insert(rope.GetTail());

    std::string segment;
    while (std::getline(instructions, segment))
    {
      const auto &[direction, value] = GetDirectionAndValue(segment);

      for (int i = 1; i <= value; ++i)
      {
        rope.MoveHead(direction);
        tailPositions.insert(rope.GetTail());
      }
    }
    return tailPositions;
  }
}

TEST_CASE("Get the rope")
{
  Rope rope;

  SECTION("Initial positions of head and tail are zeros")
  {
    Knot initial{0, 0};
    CHECK(initial == rope.GetHead());
    CHECK(initial == rope.GetTail());
  }

  SECTION("Move head up")
  {
    rope.MoveHead("U");
    CHECK(Knot{0, 1} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());
    rope.MoveHead("U");
    CHECK(Knot{0, 2} == rope.GetHead());
    CHECK(Knot{0, 1} == rope.GetTail());
  }
  SECTION("Move head right")
  {
    rope.MoveHead("R");
    CHECK(Knot{1, 0} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());

    rope.MoveHead("R");
    CHECK(Knot{2, 0} == rope.GetHead());
    CHECK(Knot{1, 0} == rope.GetTail());
  }

  SECTION("Move head left")
  {
    rope.MoveHead("L");
    CHECK(Knot{-1, 0} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());

    rope.MoveHead("L");
    CHECK(Knot{-2, 0} == rope.GetHead());
    CHECK(Knot{-1, 0} == rope.GetTail());
  }
  SECTION("Move head down")
  {
    rope.MoveHead("D");
    CHECK(Knot{0, -1} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());

    rope.MoveHead("D");
    CHECK(Knot{0, -2} == rope.GetHead());
    CHECK(Knot{0, -1} == rope.GetTail());
  }

  SECTION("Move head diagonally")
  {
    rope.MoveHead("R");
    CHECK(Knot{1, 0} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());

    rope.MoveHead("U");
    CHECK(Knot{1, 1} == rope.GetHead());
    CHECK(Knot{0, 0} == rope.GetTail());

    rope.MoveHead("U");
    CHECK(Knot{1, 2} == rope.GetHead());
    CHECK(Knot{1, 1} == rope.GetTail());
  }
}

TEST_CASE("Read instruction")
{
  std::string instruction{"U 4"};
  const auto &[direction, value] = GetDirectionAndValue(instruction);
  CHECK(direction == "U");
  CHECK(value == 4);
}

TEST_CASE("check example data")
{
  SECTION("task 1")
  {
    std::stringstream test{};
    test << "R 4\n"
         << "U 4\n"
         << "L 3\n"
         << "D 1\n"
         << "R 4\n"
         << "D 1\n"
         << "L 5\n"
         << "R 2\n";

    CHECK(13 == GetTailPositions(test).size());
  }

  SECTION("task 2")
  {
    std::stringstream test{};
    test << "R 5\n"
         << "U 8\n"
         << "L 8\n"
         << "D 3\n"
         << "R 17\n"
         << "D 10\n"
         << "L 25\n"
         << "U 20\n";

    CHECK(36 == GetTailPositions(test, 10).size());
  }
}

TEST_CASE("read day 9 data")
{
  std::fstream my_file;
  my_file.open("day9_data.txt", std::ios::in);

  SECTION("task 1")
  {
    std::cout << "Day 9 task 1 result: " << GetTailPositions(my_file).size() << std::endl;
  }

  SECTION("task 2")
  {
    std::cout << "Day 9 task 2 result: " << GetTailPositions(my_file, 10).size() << std::endl;
  }
}
