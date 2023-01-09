#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{

int FindHeaderPositionOfSize(std::basic_istream<char>& buffer, const size_t headerSize)
{
  char c;
  std::string header;
  int counter = 0;

  while (buffer.get(c))
  {
    ++counter;
    
    header += c;

    if (header.size() > headerSize )
    {
      header = header.substr(1, headerSize);
    }

    std::set<char> checker{header.begin(), header.end()};
    if (checker.size() == headerSize)
    {
      return counter;
    }
  }
  return counter;
}

}

TEST_CASE("read datastream buffer")
{
  SECTION("header size = 4")
  {
    constexpr size_t headerSize = 4;
    std::stringstream test{"mjqjpqmgbljsphdztnvjfqwrcgsmlb"};
    CHECK(7 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"bvwbjplbgvbhsrlpgdmjqwftvncz"};
    CHECK(5 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"nppdvjthqldpwncqszvftbrmjlhg"};
    CHECK(6 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"};
    CHECK(10 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"};
    CHECK(11 == FindHeaderPositionOfSize(test, headerSize));
  }

  SECTION("message size = 14")
  {
    constexpr size_t headerSize = 14;
    std::stringstream test{"mjqjpqmgbljsphdztnvjfqwrcgsmlb"};
    CHECK(19 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"bvwbjplbgvbhsrlpgdmjqwftvncz"};
    CHECK(23 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"nppdvjthqldpwncqszvftbrmjlhg"};
    CHECK(23 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"};
    CHECK(29 == FindHeaderPositionOfSize(test, headerSize));

    test = std::stringstream{"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"};
    CHECK(26 == FindHeaderPositionOfSize(test, headerSize));
  }
}

TEST_CASE("read day 6 task 1 data")
{
  constexpr size_t headerSize = 4;
  std::fstream my_file;
  my_file.open("day6_data.txt", std::ios::in);

  std::cout << "Day 6 task 1 result: " << FindHeaderPositionOfSize(my_file, headerSize) << std::endl;
}

TEST_CASE("read day 6 task 2 data")
{
  constexpr size_t headerSize = 14;
  std::fstream my_file;
  my_file.open("day6_data.txt", std::ios::in);

  std::cout << "Day 6 task 2 result: " << FindHeaderPositionOfSize(my_file, headerSize) << std::endl;
}