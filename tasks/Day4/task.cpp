#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{
class SectionAssignment
{
public:
  SectionAssignment(std::string sectionList)
  {
    const auto separatorPosition = sectionList.find("-");
    first = std::stoi(sectionList.substr(0, separatorPosition));
    last = std::stoi(sectionList.substr(separatorPosition + 1, sectionList.size()));
  }

  bool Contains(const SectionAssignment& other) const
  {
    return this->first <= other.first and this->last >= other.last;
  }

  bool HasCommonSections(const SectionAssignment& other) const
  {
    return (this->first <= other.first and this->last >= other.first) or
            (this->first <= other.last and this->last >= other.last);
  }

private:
  int first;
  int last;
};

std::pair<std::string, std::string> GetAssignmentLists(const std::string& assignmentPair)
{
  const auto separatorPosition = assignmentPair.find(",");
  return {assignmentPair.substr(0,separatorPosition), assignmentPair.substr(separatorPosition + 1, assignmentPair.size())};
}

bool IsFullyRepeated(const std::string& assignmentPair)
{
  const auto [first, second] = GetAssignmentLists(assignmentPair);
  const SectionAssignment firstList{first};
  const SectionAssignment secondList{second};

  return firstList.Contains(secondList) or secondList.Contains(firstList);
}

bool IsPartiallyRepeated(const std::string& assignmentPair)
{
  const auto [first, second] = GetAssignmentLists(assignmentPair);
  const SectionAssignment firstList{first};
  const SectionAssignment secondList{second};

  return firstList.HasCommonSections(secondList);
}

int CountFullyRepeated(std::basic_istream<char>& assignmentPairsList)
{
  int repetitions = 0;
  std::string segment{};

  while (std::getline(assignmentPairsList, segment))
  {
    repetitions += IsFullyRepeated(segment);
  }
  return repetitions;
}

int CountPartiallyRepeated(std::basic_istream<char>& assignmentPairsList)
{
  int repetitions = 0;
  std::string segment{};

  while (std::getline(assignmentPairsList, segment))
  {
    repetitions += IsPartiallyRepeated(segment);
  }
  return repetitions;
}

}

TEST_CASE("check if list contains another")
{
  CHECK(SectionAssignment("1-3").Contains(SectionAssignment("1-2")));
  CHECK(SectionAssignment("1-3").Contains(SectionAssignment("2-2")));
  CHECK(SectionAssignment("1-3").Contains(SectionAssignment("1-3")));

  CHECK_FALSE(SectionAssignment("1-3").Contains(SectionAssignment("1-4")));
  CHECK_FALSE(SectionAssignment("1-3").Contains(SectionAssignment("4-4")));
}

TEST_CASE("check if has common elements")
{
  CHECK(SectionAssignment("1-3").HasCommonSections(SectionAssignment("2-5")));
  CHECK(SectionAssignment("1-3").HasCommonSections(SectionAssignment("3-5")));

  CHECK_FALSE(SectionAssignment("1-3").HasCommonSections(SectionAssignment("4-5")));
}

TEST_CASE("split assignment lists")
{
  const auto [first, second] = GetAssignmentLists(std::string{"2-4,6-8"});
  CHECK(first == "2-4");
  CHECK(second == "6-8");
}

TEST_CASE("is any repeated")
{
  CHECK(IsFullyRepeated("22-30,25-27"));
  CHECK(IsFullyRepeated("22-30,20-31"));

  CHECK_FALSE(IsFullyRepeated("22-30,20-29"));
}

TEST_CASE("count repetitions")
{
  std::stringstream test{};
  test << "2-4,6-8\n"
        << "2-3,4-5\n"
        << "5-7,7-9\n"
        << "2-8,3-7\n"
        << "6-6,4-6\n"
        << "2-6,4-8";

  SECTION("full repetitions")
  {
    CHECK(2 == CountFullyRepeated(test));
  }

  SECTION("partial repetitions")
  {
    CHECK(4 == CountPartiallyRepeated(test));
  }
}

TEST_CASE("read day 4 task 1 data")
{
  std::fstream my_file;
  my_file.open("day4_data.txt", std::ios::in);

  std::cout << "Day 4 task 1 result: " << CountFullyRepeated(my_file) << std::endl;
}

TEST_CASE("read day 3 task 2 data")
{
  std::fstream my_file;
  my_file.open("day4_data.txt", std::ios::in);

  std::cout << "Day 4 task 2 result: " << CountPartiallyRepeated(my_file) << std::endl;
}