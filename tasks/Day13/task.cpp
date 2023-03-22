#include <catch2/catch_all.hpp>

#include <iostream>
#include <fstream>

#include <set>

namespace
{

  struct Node
  {
    Node *parent{};
    std::vector<Node *> children{};
    std::optional<int> value{std::nullopt};

    Node(Node *parent, int value) : parent(parent), value(value){};
    Node(Node *parent) : parent(parent){};

    friend std::ostream &operator<<(std::ostream &os, Node const &node)
    {
      if (node.value)
      {
        return os << *node.value;
      }
      else
      {
        os << '[';
        auto count = node.children.size();
        for (const auto& child : node.children)
        {
          os << *child;
          if (--count != 0)
          {
            os << ',';
          }
        }
        return os << ']';
      }
    }

    friend std::string Compare(int left, const Node &right)
    {
      if (right.value)
      {
        if (left == *right.value)
          return "equal";
        else if (left < *right.value)
          return "less";
        else
          return "greater";
      }
      if (right.children.empty())
      {
        return "greater";
      }
      const auto cmp = Compare(left, *right.children.at(0));
      if (cmp == "equal")
      {
        if (right.children.size() == 1)
        {
          return "equal";
        }
        else
        {
          return "less";
        }
      }
      return cmp;
    }

    friend std::string Compare(const Node &left, int right)
    {
      const auto cmp = Compare(right, left);
      if (cmp == "greater")
      {
        return "less";
      }
      if (cmp == "less")
      {
        return "greater";
      }
      return cmp;
    }

    friend std::string Compare(const Node &left, const Node &right)
    {
      if (left.value && right.value)
      {
        if (*left.value == *right.value)
          return "equal";
        else if (*left.value < *right.value)
          return "less";
        else
          return "greater";
      }
      if (left.value)
      {
        return Compare(*left.value, right);
      }
      if (right.value)
      {
        return Compare(left, *right.value);
      }

      auto it_left = left.children.begin();
      auto it_right = right.children.begin();

      while (true)
      {
        if (it_left == left.children.end() && it_right == right.children.end())
        {
          return "equal";
        }
        if (it_left == left.children.end())
        {
          return "less";
        }
        if (it_right == right.children.end())
        {
          return "greater";
        }

        const auto cmp = Compare(**it_left, **it_right);
        if (cmp != std::string("equal"))
        {
          return cmp;
        }
        it_left++;
        it_right++;
      }

      return "less";
    }
  };

  struct Packet
  {
    Packet() = default;

    Node *AddValueNode(Node *parent, int value)
    {
      nodes.push_back(std::make_unique<Node>(parent, value));
      if (parent)
      {
        parent->children.push_back(nodes.back().get());
      }
      return nodes.back().get();
    }

    Node *AddListNode(Node *parent)
    {
      nodes.push_back(std::make_unique<Node>(parent));
      if (parent)
      {
        parent->children.push_back(nodes.back().get());
      }
      return nodes.back().get();
    }

    friend std::istream &operator>>(std::istream &is, Packet &packet)
    {
      Node *current{nullptr};

      while (is)
      {
        if (std::isdigit(is.peek()))
        {
          int val{};
          is >> val;
          packet.AddValueNode(current, val);
        }
        else if (is.peek() == '[')
        {
          is.get();
          current = packet.AddListNode(current);
          if (!packet.root)
          {
            packet.root = current;
          }
        }
        else if (is.peek() == ']')
        {
          is.get();
          if (current)
          {
            current = current->parent;
          }
          if (!current)
          {
            return is;
          }
        }
        else
        {
          is.get();
        }
      }
      return is;
    }

    friend std::ostream &operator<<(std::ostream &os, Packet const &p)
    {
      if (p.root)
      {
        return os << *p.root;
      }
      return os;
    }

    friend bool operator<(const Packet &lhs, const Packet &rhs)
    {
      if (lhs.root && rhs.root)
      {
        return Compare(*lhs.root, *rhs.root) == std::string("less");
      }
      return false;
    }

    std::vector<std::unique_ptr<Node>> nodes;
    Node *root{};
  };

}

TEST_CASE("Read packets")
{
  Packet packet;

  SECTION("value node")
  {
    std::stringstream ss{"[2]"};
    ss >> packet;

    CHECK(packet.nodes.size() == 2);
    CHECK(packet.root->children.size() == 1);
    CHECK(packet.root->children.back()->value == 2);
  }

  SECTION("empty node")
  {
    std::stringstream ss{"[[]]"};
    ss >> packet;

    CHECK(packet.nodes.size() == 2);
    CHECK(packet.root->children.size() == 1);
    CHECK_FALSE(packet.root->children.back()->value);
  }

  SECTION("fix nodes")
  {
    std::stringstream ss{"[1, [1], [], [1, [2]]]"};
    ss >> packet;

    CHECK(packet.nodes.size() == 9);
    CHECK(packet.root->children.size() == 4);
  }
}

TEST_CASE("Compare packets")
{
  Packet p1;
  Packet p2;

  SECTION("value nodes")
  {
    std::stringstream ss{"[2][2,2]"};
    ss >> p1 >> p2;

    CHECK(p1 < p2);
    CHECK_FALSE(p2 < p1);
  }

  SECTION("empty nodes")
  {
    std::stringstream ss{"[][[]]"};
    ss >> p1 >> p2;

    CHECK(p1 < p2);
    CHECK_FALSE(p2 < p1);
  }

  SECTION("value and empty node")
  {
    std::stringstream ss{"[][2]"};
    ss >> p1 >> p2;

    CHECK(p1 < p2);
    CHECK_FALSE(p2 < p1);
  }

  SECTION("value and list")
  {
    std::stringstream ss{"[[1], [2,3,4]][[1], 4]"};
    ss >> p1 >> p2;

    CHECK(p1 < p2);
    CHECK_FALSE(p2 < p1);
  }

  SECTION("example")
  {
    std::stringstream ss;
    ss << "[[[10,[],5,[5,10,2,10,5]]],[[[4,1,2,5],1,2,9],[4,10],[],[]],[[],8,[[7],7]],[[[8],7,8,3]],[7,5]]\n"
       << "[[5,[[9,4],9],[7,[6,4],[1,9,9,3,10]],2,2],[[3,[],1,9]],[[8,[5],2,[10,3,8,8]],4,[],10,[5,9]],[[[],4,[7,8,6]],[[2,2,1,8,5],[0,10,6,6]],[7,[7,9,9,7],8,6]]]";
    ss >> p1 >> p2;

    CHECK_FALSE(p1 < p2);
    CHECK(p2 < p1);
  }
}

TEST_CASE("example data")
{
  std::stringstream test{};

  test << "[1,1,3,1,1]\n"
       << "[1,1,5,1,1]\n"
       << "\n"
       << "[[1],[2,3,4]]\n"
       << "[[1],4]\n"
       << "\n"
       << "[9]\n"
       << "[[8,7,6]]\n"
       << "\n"
       << "[[4,4],4,4]\n"
       << "[[4,4],4,4,4]\n"
       << "\n"
       << "[7,7,7,7]\n"
       << "[7,7,7]\n"
       << "\n"
       << "[]\n"
       << "[3]\n"
       << "\n"
       << "[[[]]]\n"
       << "[[]]\n"
       << "\n"
       << "[1,[2,[3,[4,[5,6,7]]]],8,9]\n"
       << "[1,[2,[3,[4,[5,6,0]]]],8,9]";

  SECTION("task 1")
  {
    int sum = 0;
    int it = 0;
    while (test)
    {
      Packet p1;
      Packet p2;
      ++it;
      test >> p1 >> p2;
      if (p1 < p2)
      {
        sum += it;
      }

      while (test && test.peek() != '[')
      {
        test.get();
      }
    }
    CHECK(sum == 13);
  }
}

TEST_CASE("read day 13 data")
{
  std::fstream my_file;
  my_file.open("day13_data.txt", std::ios::in);

  SECTION("task 1")
  {
    int sum = 0;
    int it = 0;
    while (my_file)
    {
      Packet p1;
      Packet p2;

      ++it;
      my_file >> p1 >> p2;

      if (p1 < p2)
      {
        sum += it;
      }
      while (my_file && my_file.peek() != '[')
      {
        my_file.get();
      }
    }
    std::cout << "Day 13 task 1 result: " << sum << std::endl;
  }

  SECTION("task 2")
  {
    std::multiset<Packet> packets;
    while (my_file)
    {
      Packet p1;
      my_file >> p1;
      packets.insert(std::move(p1));

      while (my_file && my_file.peek() != '[')
      {
        my_file.get();
      }
    }

    std::stringstream divider_1{"[[2]]"};
    std::stringstream divider_2{"[[6]]"};
    Packet p1;
    Packet p2;
    divider_1 >> p1;
    divider_2 >> p2;
    auto pos_1 = std::distance(packets.begin(), packets.insert(std::move(p1))) + 1;
    auto pos_2 = std::distance(packets.begin(), packets.insert(std::move(p2))) + 1;

    std::cout << "Day 13 task 2 result: " << pos_1 * pos_2 << std::endl;
  }
}
