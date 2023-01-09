#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{
  using Forest = std::vector<std::vector<int>>;

  class ForestAnalyzer
  {
  public:
    ForestAnalyzer(const Forest &forest) : forest(forest)
    {
      rowsCount = forest.size();
      columnCount = forest.at(0).size();
    }

    int GetVisibleTreesCount()
    {
      int visibleTrees = 0;

      for (size_t rowsCounter = 0; rowsCounter < rowsCount; ++rowsCounter)
      {
        for (size_t columnCounter = 0; columnCounter < columnCount; ++columnCounter)
        {
          if (IsVisibleFromBottom(rowsCounter, columnCounter) or
              IsVisibleFromTop(rowsCounter, columnCounter) or
              IsVisibleFromLeft(rowsCounter, columnCounter) or
              IsVisibleFromRight(rowsCounter, columnCounter))
          {
            ++visibleTrees;
          }
        }
      }
      return visibleTrees;
    }

    int FindBiggestScore()
    {
      int score = 0;

      for (size_t rowsCounter = 0; rowsCounter < rowsCount; ++rowsCounter)
      {
        for (size_t columnCounter = 0; columnCounter < columnCount; ++columnCounter)
        {
          auto treeScore = GetVisibilityScoreForTree(rowsCounter, columnCounter);
          if (treeScore > score)
          {
            score = treeScore;
          }
        }
      }
      return score;
    }

    int GetVisibilityScoreForTree(const size_t row, const size_t column)
    {
      return GetVisibilityScoreFromTop(row, column) *
             GetVisibilityScoreFromBottom(row, column) *
             GetVisibilityScoreFromLeft(row, column) *
             GetVisibilityScoreFromRight(row, column);
    }

  private:
    std::vector<int> GetRow(const size_t row)
    {
      return forest.at(row);
    }

    std::vector<int> GetColumn(const size_t column)
    {
      std::vector<int> treeColumn;
      for (const auto &row : forest)
      {
        treeColumn.push_back(row[column]);
      }
      return treeColumn;
    }

    bool IsVisibleFromLeft(const size_t row, const size_t column)
    {
      const auto &treeRow = GetRow(row);
      return std::all_of(treeRow.begin(), treeRow.begin() + static_cast<int>(column), [&](const auto &tree)
                         { return tree < treeRow[column]; });
    }

    bool IsVisibleFromRight(const size_t row, const size_t column)
    {
      const auto &treeRow = GetRow(row);
      return std::all_of(treeRow.rbegin(), treeRow.rbegin() + static_cast<int>(columnCount) - static_cast<int>(column) - 1, [&](const auto &tree)
                         { return tree < treeRow[column]; });
    }

    bool IsVisibleFromTop(const size_t row, const size_t column)
    {
      const auto &treeColumn = GetColumn(column);
      return std::all_of(treeColumn.begin(), treeColumn.begin() + static_cast<int>(row), [&](const auto &tree)
                         { return tree < treeColumn[row]; });
    }

    bool IsVisibleFromBottom(const size_t row, const size_t column)
    {
      const auto &treeColumn = GetColumn(column);
      return std::all_of(treeColumn.rbegin(), treeColumn.rbegin() + static_cast<int>(rowsCount) - static_cast<int>(row) - 1, [&](const auto &tree)
                         { return tree < treeColumn[row]; });
    }

    int GetVisibilityScoreFromTop(const size_t row, const size_t column)
    {
      const auto &treeColumn = GetColumn(column);
      return CalculateScore(std::vector<int>(treeColumn.rbegin() + static_cast<int>(rowsCount) - static_cast<int>(row) - 1, treeColumn.rend()));
    }

    int GetVisibilityScoreFromBottom(const size_t row, const size_t column)
    {
      const auto &treeColumn = GetColumn(column);
      return CalculateScore(std::vector<int>(treeColumn.begin() + static_cast<int>(row), treeColumn.end()));
    }

    int GetVisibilityScoreFromLeft(const size_t row, const size_t column)
    {
      const auto &treeRow = GetRow(row);
      return CalculateScore(std::vector<int>(treeRow.rbegin() + static_cast<int>(columnCount) - static_cast<int>(column) - 1, treeRow.rend()));
    }

    int GetVisibilityScoreFromRight(const size_t row, const size_t column)
    {
      const auto &treeRow = GetRow(row);
      return CalculateScore(std::vector<int>(treeRow.begin() + static_cast<int>(column), treeRow.end()));
    }

    int CalculateScore(std::vector<int> trees)
    {
      const auto lastVisibleTreeIt = std::find_if(trees.begin() + 1, trees.end(), [&](const auto &tree)
                                                  { return tree >= *trees.begin(); });

      int score = static_cast<int>(lastVisibleTreeIt - trees.begin());
      if (lastVisibleTreeIt == trees.end())
      {
        --score;
      }
      return score;
    }

    Forest forest;
    size_t rowsCount;
    size_t columnCount;
  };

  Forest ReadForest(std::basic_istream<char> &instructions)
  {
    Forest forest;
    std::string segment;

    while (std::getline(instructions, segment))
    {
      std::vector<int> treeRow;
      for (const auto &tree : segment)
      {
        treeRow.push_back(std::stoi(std::string{tree}));
      }
      forest.push_back(treeRow);
    }
    return forest;
  }
}

TEST_CASE("find visible trees")
{
  Forest forest{{3, 0, 3, 7, 3},
                {2, 5, 5, 1, 2},
                {6, 5, 3, 3, 2},
                {3, 3, 5, 4, 9},
                {3, 5, 3, 9, 0}};

  CHECK(21 == ForestAnalyzer(forest).GetVisibleTreesCount());

  SECTION("check visibility score")
  {
    CHECK(4 == ForestAnalyzer(forest).GetVisibilityScoreForTree(1, 2));
    CHECK(8 == ForestAnalyzer(forest).GetVisibilityScoreForTree(3, 2));
  }
}

TEST_CASE("find visible trees in example data")
{
  std::stringstream test{};
  test << "30373\n"
       << "25512\n"
       << "65332\n"
       << "33549\n"
       << "35390\n";

  const auto &forest = ReadForest(test);
  CHECK(21 == ForestAnalyzer(forest).GetVisibleTreesCount());
}

TEST_CASE("read day 8 data")
{
  std::fstream my_file;
  my_file.open("day8_data.txt", std::ios::in);

  const auto &forest = ReadForest(my_file);

  SECTION("task 1")
  {
    std::cout << "Day 8 task 1 result: " << ForestAnalyzer(forest).GetVisibleTreesCount() << std::endl;
  }

  SECTION("task 2")
  {
    std::cout << "Day 8 task 2 result: " << ForestAnalyzer(forest).FindBiggestScore() << std::endl;
  }
}
