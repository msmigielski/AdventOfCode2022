#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{

  class Command
  {
  public:
    Command(int cycles, int value) : cycles(cycles),
                                     value(value)
    {
    }

    int GetCycles() const
    {
      return cycles;
    }

    int GetValue() const
    {
      return value;
    }

  private:
    int cycles;
    int value;
  };

  class ICPUObserver
  {
  public:
    virtual void NotifyCycleAndRegisterValue(int cycles, int value) = 0;
    virtual ~ICPUObserver() = default;
  };

  class CPUObserver : public ICPUObserver
  {
  public:
    std::vector<int> GetSignalStrengths() const
    {
      return signalStrengths;
    }

  private:
    void NotifyCycleAndRegisterValue(int cycles, int value) override
    {
      signalStrengths.push_back(cycles * value);
    }

    std::vector<int> signalStrengths;
  };

  class CRT : public ICPUObserver
  {
  public:
    CRT()
    {
      const int rows = 6;
      for (int i = 0; i < rows; ++i)
      {
        crtRows.push_back(std::string(rowSize, '.'));
      }
    }

    std::vector<std::string> GetRows() const
    {
      return crtRows;
    }

    void print() const
    {
      for (const auto &row : crtRows)
      {
        std::cout << row << std::endl;
      }
    }

  private:
    void NotifyCycleAndRegisterValue(int cycles, int spritePosition) override
    {
      const int pixelNo = (cycles - 1) % static_cast<int>(rowSize);
      const size_t row = static_cast<size_t>(cycles - 1) / rowSize;
      if (pixelNo >= spritePosition - 1 and pixelNo <= spritePosition + 1)
      {
        crtRows.at(row).at(static_cast<size_t>(pixelNo)) = markedPixel;
      }
    }

    std::vector<std::string> crtRows;
    const size_t rowSize = 40;
    const char markedPixel = '#';
  };

  class CPU
  {
  public:
    void ExecuteCommand(const Command &cmd)
    {
      for (int i = 0; i < cmd.GetCycles(); ++i)
      {
        ++cycles;
        NotifyObservers();
      }
      xRegister += cmd.GetValue();
    }

    int GetCycle()
    {
      return cycles;
    }

    int GetRegisterValue()
    {
      return xRegister;
    }

    void RegisterObserverForSpecificCycles(ICPUObserver *observer, std::vector<int> subscribedCycles)
    {
      cyclesObservers.insert({observer, subscribedCycles});
    }

    void RegisterObserver(ICPUObserver *observer)
    {
      observers.insert(observer);
    }

  private:
    void NotifyObservers()
    {
      for (const auto &[observer, subscribedCycles] : cyclesObservers)
      {
        if (observer and std::find(subscribedCycles.begin(), subscribedCycles.end(), cycles) != subscribedCycles.end())
        {
          observer->NotifyCycleAndRegisterValue(cycles, xRegister);
        }
      }

      for (const auto &observer : observers)
      {
        if (observer)
        {
          observer->NotifyCycleAndRegisterValue(cycles, xRegister);
        }
      }
    }

    int cycles = 0;
    int xRegister = 1;

    std::map<ICPUObserver *, std::vector<int>> cyclesObservers;
    std::set<ICPUObserver *> observers;
  };

  Command ConvertToCommand(std::string strCommand)
  {
    if (std::string{"noop"} == strCommand)
    {
      return Command{1, 0};
    }
    else
    {
      const auto spacePosition = strCommand.find(" ");
      return Command{2, std::stoi(strCommand.substr(spacePosition, strCommand.size()))};
    }
  }

  std::vector<int> GetSignalStrengthsForCycles(std::basic_istream<char> &instructions, std::vector<int> cycles)
  {
    CPU cpu;
    CPUObserver cpuObserver;
    cpu.RegisterObserverForSpecificCycles(&cpuObserver, cycles);

    std::string segment;
    while (std::getline(instructions, segment))
    {
      const auto cmd = ConvertToCommand(segment);
      cpu.ExecuteCommand(cmd);
    }
    return cpuObserver.GetSignalStrengths();
  }

  void PrintCRTScreen(std::basic_istream<char> &instructions)
  {
    CPU cpu;
    CRT crt;
    cpu.RegisterObserver(&crt);

    std::string segment;
    while (std::getline(instructions, segment))
    {
      const auto cmd = ConvertToCommand(segment);
      cpu.ExecuteCommand(cmd);
    }
    crt.print();
  }

}

TEST_CASE("Use CPU commands")
{
  CPU cpu;

  SECTION("Get CPU initial cycle and register value")
  {
    CHECK(0 == cpu.GetCycle());
    CHECK(1 == cpu.GetRegisterValue());
  }

  SECTION("execute cmd which takes n cycles")
  {
    cpu.ExecuteCommand({1, 0});
    CHECK(1 == cpu.GetCycle());

    cpu.ExecuteCommand({5, 0});
    CHECK(6 == cpu.GetCycle());
  }

  SECTION("execute cmd which changes register value")
  {
    cpu.ExecuteCommand({1, 5});
    CHECK(6 == cpu.GetRegisterValue());

    cpu.ExecuteCommand({5, -4});
    CHECK(2 == cpu.GetRegisterValue());
  }
}

TEST_CASE("Observe register value in cycles")
{
  CPUObserver observer;
  CPU cpu;

  SECTION("idle 5 cycles")
  {
    cpu.RegisterObserverForSpecificCycles(&observer, {5});
    cpu.ExecuteCommand({5, 0});
    CHECK(observer.GetSignalStrengths() == std::vector<int>{5});
  }

  SECTION("execute commands with 2 cycles")
  {
    cpu.RegisterObserverForSpecificCycles(&observer, {5});
    cpu.ExecuteCommand({2, 1});
    cpu.ExecuteCommand({2, 1});
    cpu.ExecuteCommand({2, 1});
    CHECK(observer.GetSignalStrengths() == std::vector<int>{15});
  }

  SECTION("subscribe more cycles")
  {
    cpu.RegisterObserverForSpecificCycles(&observer, {1, 2, 5});
    cpu.ExecuteCommand({1, 0});
    cpu.ExecuteCommand({2, 1});
    cpu.ExecuteCommand({2, 1});
    CHECK(observer.GetSignalStrengths() == std::vector<int>{1, 2, 10});
  }
}

TEST_CASE("convert string to command")
{
  SECTION("noop")
  {
    const auto cmd = ConvertToCommand("noop");
    CHECK(1 == cmd.GetCycles());
    CHECK(0 == cmd.GetValue());
  }

  SECTION("addx")
  {
    const auto cmd = ConvertToCommand("addx 20");
    CHECK(2 == cmd.GetCycles());
    CHECK(20 == cmd.GetValue());
  }
}

TEST_CASE("print CRT screen")
{
  CRT crt;
  CPU cpu;
  cpu.RegisterObserver(&crt);

  cpu.ExecuteCommand({2, 15});
  CHECK("##......................................" == crt.GetRows().at(0));
}

TEST_CASE("read day 10 data")
{
  std::fstream my_file;
  my_file.open("day10_data.txt", std::ios::in);

  SECTION("task 1")
  {
    const auto signals = GetSignalStrengthsForCycles(my_file, {20, 60, 100, 140, 180, 220});
    int sum = std::accumulate(signals.begin(), signals.end(), 0);

    std::cout << "Day 10 task 1 result: " << sum << std::endl;
  }

  SECTION("task 2")
  {
    std::cout << "Day 9 task 2 result: " << std::endl;
    PrintCRTScreen(my_file);
  }
}
