/* cpsc:meta:start
snippet: fenwick-tree
cpsc:meta:end */

#include "test-support.hpp"
#include "fenwick-tree.cpp"

// cpsc:test:start
TEST(FenwickTree, BuildsAndQueriesSums) {
  FenwickTree tree(std::vector<long long>{2, 1, 3, 0, 4});

  EXPECT_EQ(tree.size(), 5);
  EXPECT_FALSE(tree.empty());
  EXPECT_EQ(tree.sum(3), 6);
  EXPECT_EQ(tree.sum(1, 5), 8);
  EXPECT_EQ(tree.get(2), 3);
  EXPECT_EQ(tree[4], 4);
}
// cpsc:test:end

// cpsc:test:start
TEST(FenwickTree, SupportsAddSetAndClear) {
  FenwickTree<> tree(4);

  tree.add(1, 5);
  tree.add(3, 2);
  tree.set(1, 3);
  EXPECT_EQ(tree.sum(0, 4), 5);
  EXPECT_EQ(tree.get(1), 3);

  tree.clear();
  EXPECT_EQ(tree.sum(4), 0);
  EXPECT_EQ(tree.size(), 4);
}
// cpsc:test:end

// cpsc:test:start
TEST(FenwickTree, FindsPrefixSumBounds) {
  FenwickTree tree(std::vector<long long>{2, 0, 3, 1});

  EXPECT_EQ(tree.lowerBound(0), 0);
  EXPECT_EQ(tree.lowerBound(1), 0);
  EXPECT_EQ(tree.lowerBound(2), 0);
  EXPECT_EQ(tree.lowerBound(3), 2);
  EXPECT_EQ(tree.lowerBound(7), tree.size());

  EXPECT_EQ(tree.upperBound(-1), 0);
  EXPECT_EQ(tree.upperBound(1), 0);
  EXPECT_EQ(tree.upperBound(2), 2);
  EXPECT_EQ(tree.upperBound(6), tree.size());
}
// cpsc:test:end

// cpsc:test:start
TEST(FenwickTree, HandlesEmptyTree) {
  FenwickTree<> tree;

  EXPECT_TRUE(tree.empty());
  EXPECT_EQ(tree.size(), 0);
  EXPECT_EQ(tree.sum(0), 0);
  EXPECT_EQ(tree.lowerBound(1), 0);
  EXPECT_EQ(tree.upperBound(0), 0);
}
// cpsc:test:end
