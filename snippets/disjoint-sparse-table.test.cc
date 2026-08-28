/* cpsc:meta:start
snippet: disjoint-sparse-table
cpsc:meta:end */

#include "test-support.hh"
#include "disjoint-sparse-table.hh"

// cpsc:test:start
TEST(DisjointSparseTable, ComputesAdditiveRangeProducts) {
  DisjointSparseTable table(std::vector<long long>{2, -1, 4, 3});

  EXPECT_EQ(table.product(0, 1), 2);
  EXPECT_EQ(table.product(0, 3), 5);
  EXPECT_EQ(table.product(1, 4), 6);
  EXPECT_EQ(table.allProduct(), 8);
  EXPECT_EQ(table.get(2), 4);
  EXPECT_EQ(table[3], 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(DisjointSparseTable, PreservesOrderForNoncommutativeOperations) {
  DisjointSparseTable<std::string> table(
      std::vector<std::string>{"a", "b", "c", "d", "e"});

  EXPECT_EQ(table.product(0, 5), "abcde");
  EXPECT_EQ(table.product(1, 4), "bcd");
  EXPECT_EQ(table.product(2, 5), "cde");
}
// cpsc:test:end

// cpsc:test:start
TEST(DisjointSparseTable, SupportsCustomAssociativeOperations) {
  auto multiply = [](long long left, long long right) { return left * right; };
  DisjointSparseTable table(std::vector{2LL, 3LL, 5LL, 7LL}, multiply);

  EXPECT_EQ(table.product(0, 2), 6);
  EXPECT_EQ(table.product(1, 4), 105);
  EXPECT_EQ(table.allProduct(), 210);
}
// cpsc:test:end

// cpsc:test:start
TEST(DisjointSparseTable, FindsPartitionPoints) {
  DisjointSparseTable table(std::vector<long long>{2, 1, 3, 2, 4});
  auto atMostSix = [](long long value) { return value <= 6; };

  EXPECT_EQ(table.partitionPoint(1, atMostSix), 4);
  EXPECT_EQ(table.partitionPoint(1, 4, atMostSix), 4);
  EXPECT_EQ(table.partitionPoint(4, atMostSix), 5);
  EXPECT_EQ(table.partitionPoint(2, 2, atMostSix), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(DisjointSparseTable, BuildsFromRangesAndRepeatedValues) {
  std::array values{1, 2, 3};
  DisjointSparseTable<int> fromRange(values);
  DisjointSparseTable<long long> repeated(4, 9);

  EXPECT_EQ(fromRange.size(), 3);
  EXPECT_EQ(fromRange.allProduct(), 6);
  EXPECT_EQ(repeated.size(), 4);
  EXPECT_EQ(repeated.allProduct(), 36);
}
// cpsc:test:end

// cpsc:test:start
TEST(DisjointSparseTable, HandlesEmptySingletonAndNonPowerOfTwoLengths) {
  DisjointSparseTable<long long> empty;
  DisjointSparseTable single(std::vector<long long>{7});
  DisjointSparseTable nonPowerOfTwo(
      std::vector<long long>{1, 2, 3, 4, 5, 6, 7});

  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(single.product(0, 1), 7);
  EXPECT_EQ(nonPowerOfTwo.product(0, 7), 28);
  EXPECT_EQ(nonPowerOfTwo.product(4, 7), 18);
}
// cpsc:test:end
