#include <catch2/catch_all.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace
{

class MealSet
{
public:
MealSet() = default;

void AddMeal(int mealKcal)
{
  mealsCalories.push_back(mealKcal);
}

int GetKcal() const
{
  return std::accumulate(mealsCalories.begin(), mealsCalories.end(), 0);
}

int GetSize() const
{
  return static_cast<int>(mealsCalories.size());
}

bool operator> (const MealSet& rhs) const
{
  return this->GetKcal() > rhs.GetKcal();
}

bool operator< (const MealSet& rhs) const
{
  return this->GetKcal() < rhs.GetKcal();
}

bool operator== (const MealSet& rhs) const
{
  return this->GetKcal() == rhs.GetKcal();
}

private:
std::vector<int> mealsCalories;
};

std::vector<MealSet> MealListToMealSets(std::basic_istream<char>& istream )
{
  std::vector<MealSet> sets{};
  MealSet currentKcalSet{};

  std::string segment{};
  while(std::getline(istream, segment))
  {
    if (segment.empty())
    {
      sets.push_back(currentKcalSet);
      currentKcalSet = {};
    }
    else
    {
      currentKcalSet.AddMeal(std::stoi(segment));
    }
  }
  if (currentKcalSet.GetSize())
  {
    sets.push_back(currentKcalSet);
  }

  return sets;
}

int GetSumOfBiggestElements(int elemCount, std::vector<MealSet> elems)
{
  std::sort(elems.begin(), elems.end());
  return std::accumulate(elems.rbegin(), elems.rbegin() + elemCount, 0, [&](auto sum, const auto& set){return sum += set.GetKcal();});
}

}


TEST_CASE("convert meal list to calories")
{
  std::stringstream test{};
  test << "123\n";
  test << "100\n";
  test << "\n";
  test << "100\n";
  test << "\n";
  test << "500\n";
  test << "100\n";
  test << "100\n";
  test << "\n";
  test << "200\n";

  auto mealSets = MealListToMealSets(test);

  SECTION("meal sets size is correct")
  {
    CHECK(4 == mealSets.size());
  }

  SECTION("get biggest calories set")
  {
    const auto biggestSet = std::max_element(mealSets.begin(), mealSets.end());
    CHECK(700 == biggestSet->GetKcal());
  }

  SECTION("get sum of 3 biggest calories set")
  {
    CHECK(1123 == GetSumOfBiggestElements(3, mealSets));
  }
}

TEST_CASE("read day 1 task 1 data")
{
  std::fstream my_file;
  my_file.open("day1_data.txt", std::ios::in);

  const auto mealSets = MealListToMealSets(my_file);
  const auto biggestSet = std::max_element(mealSets.begin(), mealSets.end());

  std::cout << "Day 1 task 1 result: " << biggestSet->GetKcal() << std::endl;
}

TEST_CASE("read day 1 task 2 data")
{
  std::fstream my_file;
  my_file.open("day1_data.txt", std::ios::in);

  const auto mealSets = MealListToMealSets(my_file);

  std::cout << "Day 1 task 2 result: " << GetSumOfBiggestElements(3, mealSets) << std::endl;
}
