#include <catch2/catch_all.hpp>

#include <fstream>
#include <iostream>
#include <stack>

namespace
{
  std::vector<int> ExtractIntegerWords(std::string str);

  class Cargo
  {
    public:
    using Stack = std::stack<char>;
    using Stacks = std::vector<Stack>;
    
    Cargo(Stacks stacks)
     : stacks(stacks)
    {
    }

    std::string GetStacksTop() const 
    {
      std::string top;
      for (const auto& stack : stacks)
      {
        if (!stack.empty())
        {
          top += stack.top();
        }
      }
      return top;
    }

    void MoveElementsByOneFromTo(size_t count, size_t from, size_t to)
    {
      for (size_t i = 0; i < count; i++)
      {
        if (!stacks.at(from).empty())
        {
          const auto elem = stacks.at(from).top();
          stacks.at(to).push(elem);
          stacks.at(from).pop();
        }
      }
    }

    void MoveElementsFromTo(size_t count, size_t from, size_t to)
    {
      std::vector<char> temp{};
      for (size_t i = 0; i < count; i++)
      {
        if (!stacks.at(from).empty())
        {
          const auto elem = stacks.at(from).top();
          stacks.at(from).pop();
          temp.push_back(elem);
        }
      }
      std::for_each(temp.rbegin(), temp.rend(), [&](const auto& elem) {stacks.at(to).push(elem);});
    }

    private:
    Stacks stacks;
  };

  class CargoFactory
  {
    public:
      static Cargo MakeCargo(std::vector<std::string> input)
      {
        constexpr auto columnSize = 3;
        constexpr auto elemPositionInColumn = 2;
        constexpr auto separatorSize = 1;
        const auto stacksCount = static_cast<size_t>(ExtractIntegerWords(input[0]).back());

        Cargo::Stacks stacks{stacksCount};

        for (const auto& line : std::vector<std::string>{input.begin()+1, input.end()})
        {
          for (size_t i = 0; i < stacksCount; i++)
          {
            const auto elem = line.at(i*columnSize + elemPositionInColumn + i * separatorSize - separatorSize);
            if(isupper(elem))
            {
              stacks.at(i).push(elem);
            }
          }
        }
        
        return stacks;
      }
  };

Cargo ReadFileAndCreateCargo(std::basic_istream<char>& inputData)
{
  std::string segment{};
  std::vector<std::string> inputCargo;

  while (std::getline(inputData, segment))
  {
    if(segment.empty())
    {
      break;
    }
    inputCargo.push_back(segment);
  }

  return CargoFactory().MakeCargo(std::vector<std::string>{inputCargo.rbegin(), inputCargo.rend()});
}

std::vector<int> ExtractIntegerWords(std::string str)
{
    std::stringstream ss{};
    ss << str;
 
    std::string temp{};
    int found{};
    std::vector<int> foundings{};

    while (!ss.eof()) {
 
        ss >> temp;
 
        if (std::stringstream(temp) >> found)
        {
          foundings.push_back(found);
        }
        temp = "";
    }
    return foundings;
}

std::string GetCargoTopPositionsWhenMovingByOne(std::basic_istream<char>& inputData)
{
  auto cargo = ReadFileAndCreateCargo(inputData);

  std::string segment{};
  while (std::getline(inputData, segment))
  {
    const auto moves = ExtractIntegerWords(segment);

    cargo.MoveElementsByOneFromTo(static_cast<size_t>(moves.at(0)), 
                                  static_cast<size_t>(moves.at(1) - 1), 
                                  static_cast<size_t>(moves.at(2) - 1));
  }
  return cargo.GetStacksTop();
}

std::string GetCargoTopPositions(std::basic_istream<char>& inputData)
{
  auto cargo = ReadFileAndCreateCargo(inputData);

  std::string segment{};
  while (std::getline(inputData, segment))
  {
    const auto moves = ExtractIntegerWords(segment);

    cargo.MoveElementsFromTo(static_cast<size_t>(moves.at(0)), 
                             static_cast<size_t>(moves.at(1) - 1), 
                             static_cast<size_t>(moves.at(2) - 1));
  }
  return cargo.GetStacksTop();
}

};


TEST_CASE("Cargo")
{
  SECTION("Create cargo stacks")
  {
    Cargo::Stack first{std::deque<char>{'A', 'A'}};
    Cargo::Stack second{std::deque<char>{'C', 'B'}};
    Cargo::Stack third{std::deque<char>{'D', 'C'}};
    Cargo::Stacks stacks{{first, second, third}};

    Cargo cargo{stacks};

    SECTION("check cargo top")
    {
      CHECK("ABC" == Cargo(stacks).GetStacksTop());
    }

    SECTION("move n elements by one from stack to stack")
    {
      cargo.MoveElementsByOneFromTo(1, 0, 1);
      cargo.MoveElementsByOneFromTo(2, 1, 2);

      CHECK("ACB" == cargo.GetStacksTop());
    }

    SECTION("move n elements from stack to stack")
    {
      cargo.MoveElementsFromTo(1, 0, 1);
      cargo.MoveElementsFromTo(2, 1, 2);

      CHECK("ACA" == cargo.GetStacksTop());
    }
  }
}

TEST_CASE("Create cargo from text input")
{
  std::vector<std::string> input{
    "    [D]    ",
    "[N] [C]    ",
    "[Z] [M] [P]",
    " 1   2   3 "
  };

  CHECK("NDP" == CargoFactory().MakeCargo({input.rbegin(), input.rend()}).GetStacksTop());
}

TEST_CASE("extract numbers from string")
{
  std::string moves{"move 1 from 2 to 1"};
  CHECK(std::vector<int>{1,2,1} == ExtractIntegerWords(moves));
}

TEST_CASE("check top positions for data example")
{
  std::stringstream test{}; 
  test << "    [D]    \n";
  test << "[N] [C]    \n";
  test << "[Z] [M] [P]\n";
  test << " 1   2   3 \n";
  test << "\n";
  test << "move 1 from 2 to 1\n";
  test << "move 3 from 1 to 3\n";
  test << "move 2 from 2 to 1\n";
  test << "move 1 from 1 to 2\n";

  SECTION("one by one")
  {
    CHECK("CMZ" == GetCargoTopPositionsWhenMovingByOne(test));
  }

  SECTION("all at once")
  {
    CHECK("MCD" == GetCargoTopPositions(test));
  }
}

TEST_CASE("read day 5 task 1 data")
{
  std::fstream my_file;
  my_file.open("day5_data.txt", std::ios::in);

  std::cout << "Day 5 task 1 result: " << GetCargoTopPositionsWhenMovingByOne(my_file) << std::endl;
}

TEST_CASE("read day 5 task 2 data")
{
  std::fstream my_file;
  my_file.open("day5_data.txt", std::ios::in);

  std::cout << "Day 5 task 2 result: " << GetCargoTopPositions(my_file) << std::endl;
}