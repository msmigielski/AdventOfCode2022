#include <catch2/catch_all.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace
{

enum Figures
{
  Rock = 1,
  Paper = 2,
  Scissors = 3
};

class HandyGame
{
  public:
    int play(Figures myFigure, Figures opponentFigure)
    {
      constexpr int winScore = 6;
      constexpr int drawScore = 3;

      int score = 0;
      score = static_cast<int>(myFigure);

      if (myFigure == GetWinningFigure(opponentFigure))
      {
        score += winScore;
      }
      else if (myFigure == opponentFigure)
      {
        score += drawScore;
      }
      return score;
    };

    static Figures GetWinningFigure(Figures figure)
    {
      if (figure == Figures::Rock)
      {
        return Figures::Paper;
      }
      else if (figure == Figures::Paper)
      {
        return Figures::Scissors;
      }
      else
      {
        return Figures::Rock;
      }
    }

    static Figures GetLoosingFigure(Figures figure)
    {
      if (figure == Figures::Rock)
      {
        return Figures::Scissors;
      }
      else if (figure == Figures::Paper)
      {
        return Figures::Rock;
      }
      else
      {
        return Figures::Paper;
      }
    }    
};

std::map<char, Figures> encodedFigures
{
  {'A', Figures::Rock},
  {'X', Figures::Rock},
  {'B', Figures::Paper},
  {'Y', Figures::Paper},
  {'C', Figures::Scissors},
  {'Z', Figures::Scissors},
};

Figures ConvertCharToFigure(char c)
{
  return encodedFigures[c];
}

std::pair<Figures, Figures> ConvertStrategyToFigures(char first, char second)
{
  Figures oponentFigure = encodedFigures[first];
  
  Figures myFigure{};
  if (second == 'X')
  {
    myFigure = HandyGame::GetLoosingFigure(oponentFigure);
  }
  else if (second == 'Y')
  {
    myFigure = oponentFigure;
  }
  else
  {
    myFigure = HandyGame::GetWinningFigure(oponentFigure);
  }

  return {oponentFigure, myFigure};
}

int PlayGame(std::basic_istream<char>& istream )
{
  int score = 0;
  std::string segment;
  HandyGame game;

  while(std::getline(istream, segment))
  {
    score += game.play(ConvertCharToFigure(segment[2]), ConvertCharToFigure(segment[0]));
  }

  return score;
}

int PlayGameWithStrategy(std::basic_istream<char>& istream )
{
  int score = 0;
  std::string segment;
  HandyGame game;

  while(std::getline(istream, segment))
  {
    auto [oponentFigure, myFigure] = ConvertStrategyToFigures(segment[0], segment[2]);
    score += game.play(myFigure, oponentFigure);
  }

  return score;
}

}

TEST_CASE("Play game")
{
  HandyGame game;
  SECTION("Score played figure")
  {
    CHECK(1 == game.play(Figures::Rock, Figures::Paper));
    CHECK(2 == game.play(Figures::Paper, Figures::Scissors));
    CHECK(3 == game.play(Figures::Scissors, Figures::Rock));
  }

  SECTION("Score win game")
  {
    CHECK(7 == game.play(Figures::Rock, Figures::Scissors));
    CHECK(8 == game.play(Figures::Paper, Figures::Rock));
    CHECK(9 == game.play(Figures::Scissors, Figures::Paper));
  }

  SECTION("Score draw game")
  {
    CHECK(4 == game.play(Figures::Rock, Figures::Rock));
    CHECK(5 == game.play(Figures::Paper, Figures::Paper));
    CHECK(6 == game.play(Figures::Scissors, Figures::Scissors));
  }
}

TEST_CASE("Convert input to figures")
{
  SECTION("Convert char to figure")
  {
    CHECK(Figures::Rock == ConvertCharToFigure('A'));
    CHECK(Figures::Rock == ConvertCharToFigure('X'));
    CHECK(Figures::Paper == ConvertCharToFigure('B'));
    CHECK(Figures::Paper == ConvertCharToFigure('Y'));
    CHECK(Figures::Scissors == ConvertCharToFigure('C'));
    CHECK(Figures::Scissors == ConvertCharToFigure('Z'));
  }

  SECTION("Convert chars to figures with strategy")
  {
    auto [oponentFigure, myFigure] = ConvertStrategyToFigures('A', 'X');
    CHECK(Figures::Rock == oponentFigure);
    CHECK(Figures::Scissors == myFigure);

    auto [oponentFigure2, myFigure2] = ConvertStrategyToFigures('A', 'Y');
    CHECK(Figures::Rock == oponentFigure2);
    CHECK(Figures::Rock == myFigure2);

    auto [oponentFigure3, myFigure3] = ConvertStrategyToFigures('A', 'Z');
    CHECK(Figures::Rock == oponentFigure3);
    CHECK(Figures::Paper == myFigure3);
  }
}

TEST_CASE("Play game from test input")
{
  SECTION("one by one")
  {
    HandyGame game;
    CHECK(8 == game.play(ConvertCharToFigure('Y'), ConvertCharToFigure('A')));
    CHECK(1 == game.play(ConvertCharToFigure('X'), ConvertCharToFigure('B')));
    CHECK(6 == game.play(ConvertCharToFigure('Z'), ConvertCharToFigure('C')));
  }

  SECTION("whole game")
  {
    std::stringstream test{};
    test << "A Y\n";
    test << "B X\n";
    test << "C Z";
    CHECK(15 == PlayGame(test));
  }

  SECTION("play game with strategy")
  {
    std::stringstream test{};
    test << "A Y\n";
    test << "B X\n";
    test << "C Z";
    CHECK(12 == PlayGameWithStrategy(test));
  }
}

TEST_CASE("read day 2 task 1 data")
{
  std::fstream my_file;
  my_file.open("day2_data.txt", std::ios::in);

  std::cout << "Day 2 task 1 result: " << PlayGame(my_file) << std::endl;
}

TEST_CASE("read day 2 task 2 data")
{
  std::fstream my_file;
  my_file.open("day2_data.txt", std::ios::in);

  std::cout << "Day 2 task 2 result: " << PlayGameWithStrategy(my_file) << std::endl;
}