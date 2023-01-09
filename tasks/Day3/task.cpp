#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{
  std::pair<std::string, std::string> SplitToCompartments(std::string items)
  {
    const auto part1 = items.substr(0, items.size()/2);
    const auto part2 = items.substr(items.size()/2, items.size());
    return {part1, part2};
  }

  char GetCommonItem(std::vector<std::string> backpacks)
  {
    if(backpacks.size() < 2)
    {
      return {};
    }

    for (const auto& item : backpacks[0])
    {
      const bool isCommon = std::all_of(backpacks.begin()+1, backpacks.end(),
                                       [&item](const auto& backpack){ return backpack.find(item) != std::string::npos; });
      if (isCommon)
      {
        return item;
      }
    }

    return {};
  }

  int CalculateItemPriority(char item)
  {
    constexpr int firstValueForUpper = 27;
    constexpr int firstValueForLower = 1;

    if (isupper(item))
    {
      return static_cast<int>(item) - static_cast<int>('A') + firstValueForUpper;
    }
    return static_cast<int>(item) - static_cast<int>('a') + firstValueForLower;
  }

  int CalculatePriorityForBackpack(std::string items)
  {
    const auto [part1, part2] = SplitToCompartments(items);
    const auto item = GetCommonItem({part1, part2});
    return CalculateItemPriority(item);
  }

  int CalculatePriorityForBackpacks(std::basic_istream<char>& backpacks)
  {
    int priority = 0;
    std::string items;

    while(std::getline(backpacks, items))
    {
      priority += CalculatePriorityForBackpack(items);
    }

    return priority;
  }

  int CalculateGroupPriorityForBackpacks(std::basic_istream<char>& backpacks)
  {
    int priority = 0;
    std::vector<std::string>::size_type iter = 0;
    std::string items;
    std::vector<std::string> group{3};

    while(std::getline(backpacks, items))
    {
      group[iter] = items;

      if (iter < 2)
      {
        ++iter;
      }
      else
      {
        const auto item = GetCommonItem(group);
        priority += CalculateItemPriority(item);
        iter = 0;
      }
    }

    return priority;
  }
}

TEST_CASE("split items for compartments")
{
  std::string items{"vJrwpWtwJgWrhcsFMMfFFhFp"};
  auto [part1, part2] = SplitToCompartments(items);
  CHECK(part1 == std::string{"vJrwpWtwJgWr"});
  CHECK(part2 == std::string{"hcsFMMfFFhFp"});
}

TEST_CASE("find common item")
{
  std::string items1{"vJrwpWtwJgWr"};
  std::string items2{"hcsFMMfFFhFp"};

  const auto item = GetCommonItem({items1, items2});
  CHECK(item == 'p');
}

TEST_CASE("calculate priority for item")
{
  CHECK(42 == CalculateItemPriority('P'));
  CHECK(16 == CalculateItemPriority('p'));
}

TEST_CASE("calculate priority for backpack")
{
  std::string items{"vJrwpWtwJgWrhcsFMMfFFhFp"};
  CHECK(16 == CalculatePriorityForBackpack(items));
}

TEST_CASE("calculate priorities for backpack")
{
  std::stringstream test{};
  test << "vJrwpWtwJgWrhcsFMMfFFhFp\n"
       << "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL\n"
       << "PmmdzqPrVvPwwTWBwg\n"
       << "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn\n"
       << "ttgJtRGJQctTZtZT\n"
       << "CrZsJsPPZsGzwwsLwLmpwMDw";
  CHECK(157 == CalculatePriorityForBackpacks(test));
}

TEST_CASE("calculate priorities for group")
{
  std::stringstream test{};
  test << "vJrwpWtwJgWrhcsFMMfFFhFp\n" 
       << "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL\n" 
       << "PmmdzqPrVvPwwTWBwg\n";
  CHECK(18 == CalculateGroupPriorityForBackpacks(test));
}

TEST_CASE("read day 3 task 1 data")
{
  std::fstream my_file;
  my_file.open("day3_data.txt", std::ios::in);

  std::cout << "Day 3 task 1 result: " << CalculatePriorityForBackpacks(my_file) << std::endl;
}

TEST_CASE("read day 3 task 2 data")
{
  std::fstream my_file;
  my_file.open("day3_data.txt", std::ios::in);

  std::cout << "Day 3 task 2 result: " << CalculateGroupPriorityForBackpacks(my_file) << std::endl;
}

