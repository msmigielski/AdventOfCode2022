#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>
#include <list>
#include <cstdint>

namespace
{
  class Monkey
  {
  public:
    Monkey(std::list<uint64_t> items,
           std::function<uint64_t(uint64_t)> investigationOperation,
           std::function<uint64_t(uint64_t)> testOperation) : items(items),
                                                    investigationOperation(investigationOperation),
                                                    testOperation(testOperation)
    {
    }

    virtual ~Monkey() = default;

    bool HasItems() const
    {
      return !items.empty();
    }

    void AddItem(uint64_t item)
    {
      items.push_back(item);
    }

    uint64_t DropCurrentItem()
    {
      const auto currentItem = items.front();
      items.pop_front();
      return currentItem;
    }

    void InvestigateCurrentItem()
    {
      auto &currentItem = items.front();
      currentItem = investigationOperation(currentItem);
      currentItem = ReduceWorryLevel(currentItem);
    }

    uint64_t GetRecipientMonkey() const
    {
      return testOperation(items.front());
    }

  protected:
    virtual uint64_t ReduceWorryLevel(uint64_t item)
    {
      const uint8_t worryDivisor = 3;
      return item /= worryDivisor;
    }


  private:
    std::list<uint64_t> items;
    std::function<uint64_t(uint64_t)> investigationOperation;
    std::function<uint64_t(uint64_t)> testOperation;
  };

  class StressfulMonkey : public Monkey
  {
  public:
    StressfulMonkey(std::list<uint64_t> items,
                    std::function<uint64_t(uint64_t)> investigationOperation,
                    std::function<uint64_t(uint64_t)> testOperation) : Monkey(items, investigationOperation, testOperation){};

    void SetReduceWorryLevelFactor(uint64_t reduceWorryLevelFactor)
    {
      worryLevelFactor = reduceWorryLevelFactor;
    }

  private:
    uint64_t ReduceWorryLevel(uint64_t item) override
    {
      return item % worryLevelFactor;
    }

    uint64_t worryLevelFactor = 1;
  };

  template <typename T = Monkey>
  class MonkeyGame
  {
  public:
    MonkeyGame(std::vector<T> monkeys) : monkeys(monkeys),
                                         monkeysActivity(monkeys.size()) {}

    void PlayRound()
    {
      size_t monkeyNo = 0;
      for (auto &monkey : monkeys)
      {
        while (monkey.HasItems())
        {
          ++monkeysActivity.at(monkeyNo);
          monkey.InvestigateCurrentItem();
          monkeys.at(static_cast<size_t>(monkey.GetRecipientMonkey())).AddItem(monkey.DropCurrentItem());
        }
        ++monkeyNo;
      }
    }

    std::vector<uint64_t> GetMonkeysActivity() const
    {
      return monkeysActivity;
    }

  private:
    std::vector<T> monkeys;
    std::vector<uint64_t> monkeysActivity;
  };

  std::vector<uint64_t> ExtractIntegerWords(std::string str)
  {
    std::stringstream ss{};
    ss << str;

    std::string temp{};
    uint64_t found{};
    std::vector<uint64_t> foundings{};

    while (!ss.eof())
    {

      ss >> temp;

      if (std::stringstream(temp) >> found)
      {
        foundings.push_back(found);
      }
      temp = "";
    }
    return foundings;
  }

  std::list<uint64_t> ReadStartingItems(std::basic_istream<char> &input)
  {
    std::string segment;
    std::getline(input, segment);
    auto items = ExtractIntegerWords(segment);
    return {items.begin(), items.end()};
  }

  std::function<uint64_t(uint64_t)> ReadOperationFunction(std::basic_istream<char> &items)
  {
    std::string segment;
    std::getline(items, segment);
    const auto factors = ExtractIntegerWords(segment);
    if (factors.empty())
    {
      return [](uint64_t item)
      { return std::pow(item, 2); };
    }
    else if (segment.find("*") != std::string::npos)
    {
      return [factors](uint64_t item)
      { return item * factors.at(0); };
    }
    else if (segment.find("+") != std::string::npos)
    {
      return [factors](uint64_t item)
      { return item + factors.at(0); };
    }
    return [](uint64_t item)
    { return item; };
  }

  std::function<uint64_t(uint64_t)> ReadTestFunction(std::basic_istream<char> &items)
  {
    std::string segment;
    std::getline(items, segment);
    const auto factor = ExtractIntegerWords(segment).at(0);

    std::getline(items, segment);
    const auto trueValue = ExtractIntegerWords(segment).at(0);

    std::getline(items, segment);
    const auto falseValue = ExtractIntegerWords(segment).at(0);

    return [factor, trueValue, falseValue](uint64_t item)
    { return item % factor == 0 ? trueValue : falseValue; };
  }

  template <typename T = Monkey>
  T ReadMonkey(std::basic_istream<char> &input)
  {
    return T{
        ReadStartingItems(input),
        ReadOperationFunction(input),
        ReadTestFunction(input)};
  }

  template <typename T = Monkey>
  std::vector<T> ReadMonkeys(std::basic_istream<char> &input)
  {
    std::vector<T> monkeys;
    std::string segment;
    while (std::getline(input, segment))
    {
      if (segment.find("Monkey") != std::string::npos)
      {
        monkeys.push_back(ReadMonkey<T>(input));
      }
    }

    input.clear();
    input.seekg(0);

    return monkeys;
  }

  uint64_t ReadLeastCommonMultipleOfTestDivisors(std::basic_istream<char> &input)
  {
    uint64_t leastCommonMultiple = 1;

    std::string segment;
    while (std::getline(input, segment))
    {
      if (segment.find("Test: divisible by") != std::string::npos)
      {
        leastCommonMultiple = std::lcm(leastCommonMultiple, ExtractIntegerWords(segment).at(0));
      }
    }

    input.clear();
    input.seekg(0);

    return leastCommonMultiple;
  }

}

TEST_CASE("Create Monkey")
{
  Monkey monkey{{},
                [](uint64_t item)
                { return item * 10; },
                [](uint64_t)
                { return 1; }};

  SECTION("Check monkeys items")
  {
    CHECK_FALSE(monkey.HasItems());

    const uint64_t testItem = 5;
    monkey.AddItem(testItem);
    CHECK(monkey.HasItems());

    CHECK(monkey.DropCurrentItem() == 5);
    CHECK_FALSE(monkey.HasItems());
  }

  SECTION("investigate item")
  {
    monkey.AddItem(5);
    monkey.InvestigateCurrentItem();
    CHECK(monkey.DropCurrentItem() == 16);
  }

  SECTION("get recipient monkey")
  {
    monkey.AddItem(5);
    CHECK(monkey.GetRecipientMonkey() == 1);
  }
}

TEST_CASE("Play example game")
{
  MonkeyGame game{{
      {{79, 98}, [](uint64_t item)
       { return item * 19; },
       [](uint64_t item)
       { return item % 23 == 0 ? 2 : 3; }},
      {{54, 65, 75, 74}, [](uint64_t item)
       { return item + 6; },
       [](uint64_t item)
       { return item % 19 == 0 ? 2 : 0; }},
      {{79, 60, 97}, [](uint64_t item)
       { return item * item; },
       [](uint64_t item)
       { return item % 13 == 0 ? 1 : 3; }},
      {{74}, [](uint64_t item)
       { return item + 3; },
       [](uint64_t item)
       { return item % 17 == 0 ? 0 : 1; }},
  }};

  for (uint64_t i = 1; i <= 20; ++i)
  {
    game.PlayRound();
  }

  CHECK(game.GetMonkeysActivity() == std::vector<uint64_t>{101, 95, 7, 105});
}

TEST_CASE("read input data")
{
  std::stringstream test{};

  SECTION("read starting items")
  {
    test << "  Starting items: 79, 98\n";
    CHECK(std::list<uint64_t>{79, 98} == ReadStartingItems(test));
  }

  SECTION("read operation")
  {
    SECTION("multiply")
    {
      test << "  Operation: new = old * 19\n";
      auto operation = ReadOperationFunction(test);
      CHECK(19 == operation(1));
    }

    SECTION("pow")
    {
      test << "  Operation: new = old * old\n";
      auto operation = ReadOperationFunction(test);
      CHECK(4 == operation(2));
    }

    SECTION("addition")
    {
      test << "  Operation: new = old + 5\n";
      auto operation = ReadOperationFunction(test);
      CHECK(7 == operation(2));
    }
  }

  SECTION("read test function")
  {
    test << "  Test: divisible by 19\n"
         << "     If true: throw to monkey 2\n"
         << "     If false: throw to monkey 0\n";

    auto operation = ReadTestFunction(test);

    CHECK(2 == operation(19));
    CHECK(0 == operation(18));
  }

  SECTION("Read Least Common Multiple Of Test Divisors")
  {
    test << "Test: divisible by 23\n"
         << "Test: divisible by 19\n"
         << "Test: divisible by 13\n"
         << "Test: divisible by 17\n";

    CHECK(96577 == ReadLeastCommonMultipleOfTestDivisors(test));
  }
}

TEST_CASE("read day 11 data")
{
  std::fstream my_file;
  my_file.open("day11_data.txt", std::ios::in);

  SECTION("task 1")
  {
    MonkeyGame game{ReadMonkeys(my_file)};
    for (uint64_t i = 0; i < 20; ++i)
    {
      game.PlayRound();
    }

    auto activities = game.GetMonkeysActivity();
    std::sort(activities.begin(), activities.end());

    std::cout << "Day 11 task 1 result: " << activities.at(activities.size() - 1) * activities.at(activities.size() - 2) << std::endl;
  }

  SECTION("task 2")
  {
    uint64_t leastCommonMultiple = ReadLeastCommonMultipleOfTestDivisors(my_file);

    CHECK(leastCommonMultiple == 9699690);

    auto monkeys = ReadMonkeys<StressfulMonkey>(my_file);
    for (auto &monkey : monkeys)
    {
      monkey.SetReduceWorryLevelFactor(leastCommonMultiple);
    }

    MonkeyGame game{monkeys};
    for (uint64_t i = 0; i < 10000; ++i)
    {
      game.PlayRound();
    }

    auto activities = game.GetMonkeysActivity();
    std::sort(activities.begin(), activities.end());

    std::cout << "Day 11 task 2 result: " << activities.at(activities.size() - 1) * activities.at(activities.size() - 2) << std::endl;
  }
}
