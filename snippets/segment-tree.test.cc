/* cpsc:meta:start
snippet: segment-tree
cpsc:meta:end */

#include "test-support.hh"
#include "segment-tree.hh"

// cpsc:test:start
TEST(SegmentTree, BuildsQueriesAndUpdatesProducts) {
  SegmentTree tree(std::vector<long long>{2, 1, 3, 4, 2});

  EXPECT_EQ(tree.size(), 5);
  EXPECT_FALSE(tree.empty());
  EXPECT_EQ(tree.product(0, 5), 12);
  EXPECT_EQ(tree.product(1, 4), 8);
  EXPECT_EQ(tree.product(2, 2), 0);
  EXPECT_EQ(tree.allProduct(), 12);
  EXPECT_EQ(tree.get(2), 3);
  EXPECT_EQ(tree[4], 2);

  tree.set(2, 10);
  EXPECT_EQ(tree.product(1, 4), 15);
  EXPECT_EQ(tree.allProduct(), 19);
}
// cpsc:test:end

// cpsc:test:start
TEST(SegmentTree, PreservesOrderForNoncommutativeOperations) {
  auto concat = [](const std::string &a, const std::string &b) {
    return a + b;
  };
  SegmentTree<std::string, decltype(concat)> tree(
      std::vector<std::string>{"a", "b", "c", "d"}, "", concat);

  EXPECT_EQ(tree.product(0, 4), "abcd");
  EXPECT_EQ(tree.product(1, 3), "bc");
  tree.set(2, "X");
  EXPECT_EQ(tree.allProduct(), "abXd");
}
// cpsc:test:end

// cpsc:test:start
TEST(SegmentTree, FindsPartitionPointsInBothDirections) {
  SegmentTree tree(std::vector<long long>{2, 1, 3, 4, 2});
  auto atMostSix = [](long long value) { return value <= 6; };
  auto atMostFour = [](long long value) { return value <= 4; };

  EXPECT_EQ(tree.partitionPoint(0, atMostSix), 3);
  EXPECT_EQ(tree.partitionPoint(1, 4, atMostFour), 3);
  EXPECT_EQ(tree.partitionPoint(2, 2, atMostSix), 2);
  EXPECT_EQ(tree.partitionPointReverse(5, atMostSix), 3);
  EXPECT_EQ(tree.partitionPointReverse(1, 4, atMostFour), 3);
  EXPECT_EQ(tree.partitionPointReverse(2, 2, atMostSix), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(SegmentTree, HandlesEmptySingletonAndClear) {
  SegmentTree<long long> empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(empty.product(0, 0), 0);
  EXPECT_EQ(empty.allProduct(), 0);

  SegmentTree tree(std::vector<long long>{7});
  EXPECT_EQ(tree.product(0, 1), 7);
  EXPECT_EQ(tree.partitionPoint(0, [](long long x) { return x < 7; }), 0);
  EXPECT_EQ(tree.partitionPointReverse(1, [](long long x) { return x < 7; }),
            1);

  tree.clear();
  EXPECT_EQ(tree.get(0), 0);
  EXPECT_EQ(tree.allProduct(), 0);
}
// cpsc:test:end
