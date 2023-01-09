#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

namespace
{
  struct Token
  {
    enum Type
    {
      ChangeDir,
      List,
      Directory,
      File
    } type;
    std::string value;

    Token(Type type, std::string value) : type(type), value(value)
    {
    }

    bool operator==(const Token &other) const
    {
      return this->type == other.type and this->value == other.value;
    }
  };

  class Lexer
  {
  public:
    Token lex(std::string instruction)
    {
      if (const std::string changeDirInstruction{"$ cd "};
          instruction.find(changeDirInstruction) != std::string::npos)
      {
        return {Token::ChangeDir, instruction.substr(changeDirInstruction.size())};
      }
      else if (const std::string listInstruction("$ ls");
               instruction.find(listInstruction) != std::string::npos)
      {
        return {Token::List, {}};
      }
      else if (const std::string directoryInstruction("dir ");
               instruction.find(directoryInstruction) == 0 and instruction.find(directoryInstruction) != std::string::npos)
      {
        return {Token::Directory, {instruction.substr(directoryInstruction.size())}};
      }
      else
      {
        return {Token::File, instruction.substr(0, instruction.find(" "))};
      }
    }
  };

  using DirectoryToSize = std::map<std::string, int>;

  class Parser
  {
  public:
    Parser() = default;

    DirectoryToSize parseToPathsAndDirSizes(const std::vector<Token> tokens)
    {
      ParseTokens(tokens);
      AddInnerDirSizesToOuterDirs();
      return dirsToSizes;
    }

  private:
    void ParseTokens(const std::vector<Token> tokens)
    {
      for (const auto &token : tokens)
      {
        if (token.type == Token::ChangeDir)
        {
          ParseChangeDirToken(token);
        }
        else if (token.type == Token::File)
        {
          ParseFileToken(token);
        }
      }
    }

    void ParseChangeDirToken(const Token &token)
    {
      if (token.value == changeDirToPrevious)
      {
        ChangeCurrentDirectoryToPrevious();
      }
      else
      {
        AddDirectoryToCurrentPath(token.value);
        dirsToSizes.insert({currentPath, {}});
      }
    }

    void AddDirectoryToCurrentPath(const std::string &directory)
    {
      if (!currentPath.empty() and currentPath != dirSeparator)
      {
        currentPath += dirSeparator + directory;
      }
      else
      {
        currentPath += directory;
      }
    }

    void ChangeCurrentDirectoryToPrevious()
    {
      currentPath = currentPath.substr(0, currentPath.rfind(dirSeparator));
      if (currentPath.empty())
      {
        currentPath = "/";
      }
    }

    void ParseFileToken(const Token &token)
    {
      dirsToSizes.at(currentPath) += std::stoi(token.value);
    }

    void AddInnerDirSizesToOuterDirs()
    {
      for (auto &[outerPath, outerDirSize] : dirsToSizes)
      {
        for (const auto &[innerPath, innerDirSize] : dirsToSizes)
        {
          if ( innerPath.find(outerPath) == 0 and outerPath != innerPath)
          {
            outerDirSize += innerDirSize;
          }
        }
      }
    }

    std::string currentPath{};
    DirectoryToSize dirsToSizes;

    const std::string dirSeparator{"/"};
    const std::string changeDirToPrevious{".."};
  };

}

TEST_CASE("change instructions to tokens")
{
  Lexer lexer;

  SECTION("change dir")
  {
    CHECK(Token{Token::ChangeDir, {".."}} == lexer.lex("$ cd .."));
  }
  SECTION("list")
  {
    CHECK(Token{Token::List, {""}} == lexer.lex("$ ls"));
  }
  SECTION("directory")
  {
    CHECK(Token{Token::Directory, {"bsjbvff"}} == lexer.lex("dir bsjbvff"));
  }
  SECTION("file")
  {
    CHECK(Token{Token::File, {"221301"}} == lexer.lex("221301 nrcg.pqw"));
  }
}

TEST_CASE("parse tokens")
{
  std::vector<Token> tokens{{Token::ChangeDir, {"/"}}};

  Parser parser;

  SECTION("parse directories")
  {
    tokens.push_back({Token::ChangeDir, {"a"}});
    tokens.push_back({Token::ChangeDir, {"b"}});
    tokens.push_back({Token::ChangeDir, {".."}});
    tokens.push_back({Token::ChangeDir, {"c"}});
    tokens.push_back({Token::ChangeDir, {".."}});
    tokens.push_back({Token::ChangeDir, {".."}});
    tokens.push_back({Token::ChangeDir, {"d"}});

    DirectoryToSize ExpectedDirToSize{
        {"/", {}},
        {"/a", {}},
        {"/a/b", {}},
        {"/a/c", {}},
        {"/d", {}}};

    CHECK(parser.parseToPathsAndDirSizes(tokens) == ExpectedDirToSize);
  }

  SECTION("parse directories' sizes")
  {
    tokens.push_back({Token::ChangeDir, "a"});
    tokens.push_back({Token::List, {}});
    tokens.push_back({Token::File, "1234"});
    tokens.push_back({Token::ChangeDir, ".."});
    tokens.push_back({Token::ChangeDir, "b"});
    tokens.push_back({Token::File, "4321"});

    DirectoryToSize ExpectedDirToSize{
        {"/", 1234 + 4321},
        {"/a", 1234},
        {"/b", 4321}};

    CHECK(parser.parseToPathsAndDirSizes(tokens) == ExpectedDirToSize);
  }
}

DirectoryToSize ParseInstructions(std::basic_istream<char> &instructions)
{
  std::string instruction{};
  Lexer lexer;
  Parser parser;
  std::vector<Token> tokens;

  while (std::getline(instructions, instruction))
  {
    tokens.push_back(lexer.lex(instruction));
  }

  return parser.parseToPathsAndDirSizes(tokens);
}

int GetSizeOfRequiredSpace(const DirectoryToSize &dirsToSizes)
{
  constexpr int totalSpace = 70000000;
  constexpr int updateSpace = 30000000;

  const int usedSpace = dirsToSizes.at("/");

  const int unusedSpace = totalSpace - usedSpace;
  const int spaceToFree = updateSpace - unusedSpace;
  return spaceToFree;
}

TEST_CASE("lexer and parser integration")
{
  std::stringstream test{};
  test << "$ cd /\n"
       << "$ ls\n"
       << "dir a\n"
       << "14848514 b.txt\n"
       << "8504156 c.dat\n"
       << "dir d\n"
       << "$ cd a\n"
       << "$ ls\n"
       << "dir e\n"
       << "29116 f\n"
       << "2557 g\n"
       << "62596 h.lst\n"
       << "$ cd e\n"
       << "$ ls\n"
       << "584 i\n"
       << "$ cd ..\n"
       << "$ cd ..\n"
       << "$ cd d\n"
       << "$ ls\n"
       << "4060174 j\n"
       << "8033020 d.log\n"
       << "5626152 d.ext\n"
       << "7214296 k\n";

  const auto &dirsToSizes = ParseInstructions(test);

  CHECK(dirsToSizes.at("/a") == 94853);
  CHECK(dirsToSizes.at("/a/e") == 584);

  CHECK(2 == std::count_if(dirsToSizes.begin(), dirsToSizes.end(), [&](const auto &dirAndSize)
                           { return dirAndSize.second <= 100000; }));

  CHECK(GetSizeOfRequiredSpace(dirsToSizes) == 8381165);
}

TEST_CASE("read day 7 data")
{
  std::fstream my_file;
  my_file.open("day7_data.txt", std::ios::in);

  const auto &dirsToSizes = ParseInstructions(my_file);

  SECTION("task 1")
  {
    int sum = std::accumulate(dirsToSizes.begin(), dirsToSizes.end(), 0, [&](auto sum, const auto &dirAndSize)
                              { return sum += dirAndSize.second <= 100000 ? dirAndSize.second : 0; });

    std::cout << "Day 7 task 1 result: " << sum << std::endl;
  }

  SECTION("task 2")
  {
    const int requiredSpace = GetSizeOfRequiredSpace(dirsToSizes);
    std::vector<int> bigDirectoriesSizes;
    for (const auto &[key, value] : dirsToSizes)
    {
      if (value >= requiredSpace)
      {
        bigDirectoriesSizes.push_back(value);
      }
    }
    const auto &smallestPossibleDirSize = std::min_element(bigDirectoriesSizes.begin(), bigDirectoriesSizes.end());

    std::cout << "Day 7 task 2 result: " << *smallestPossibleDirSize << std::endl;
  }
}
