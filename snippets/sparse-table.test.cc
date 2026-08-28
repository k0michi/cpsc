/* cpsc:meta:start
snippet: sparse-table
cpsc:meta:end */

#include "test-support.hh"
#include "sparse-table.hh"

namespace sparseTableTest {
struct Minimum {
  int operator()(int left, int right) const {
    return std::min(left, right);
  }
};

struct LeftProjection {
  int operator()(int left, int) const {
    return left;
  }
};
} // namespace sparseTableTest

// cpsc:test:start
TEST(SparseTable, ComputesRangeMinimums) {
  using sparseTableTest::Minimum;
  SparseTable<int, Minimum> table(std::vector{5, 2, 6, 1, 3});

  EXPECT_EQ(table.product(0, 1), 5);
  EXPECT_EQ(table.product(0, 3), 2);
  EXPECT_EQ(table.product(1, 5), 1);
  EXPECT_EQ(table.allProduct(), 1);
  EXPECT_EQ(table.get(2), 6);
  EXPECT_EQ(table[4], 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(SparseTable, SupportsOtherIdempotentOperations) {
  auto gcd = [](int left, int right) { return std::gcd(left, right); };
  SparseTable table(std::vector{24, 36, 54, 30}, gcd);

  EXPECT_EQ(table.product(0, 2), 12);
  EXPECT_EQ(table.product(1, 4), 6);
  EXPECT_EQ(table.allProduct(), 6);
}
// cpsc:test:end

// cpsc:test:start
TEST(SparseTable, DoesNotRequireCommutativity) {
  using sparseTableTest::LeftProjection;
  SparseTable<int, LeftProjection> table(std::vector{4, 3, 2, 1});

  EXPECT_EQ(table.product(0, 4), 4);
  EXPECT_EQ(table.product(1, 4), 3);
  EXPECT_EQ(table.product(2, 4), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(SparseTable, FindsPartitionPoints) {
  using sparseTableTest::Minimum;
  SparseTable<int, Minimum> table(std::vector{8, 7, 5, 6, 3, 4});
  auto atLeastFive = [](int value) { return value >= 5; };

  EXPECT_EQ(table.partitionPoint(0, atLeastFive), 4);
  EXPECT_EQ(table.partitionPoint(1, 5, atLeastFive), 4);
  EXPECT_EQ(table.partitionPoint(4, atLeastFive), 4);
  EXPECT_EQ(table.partitionPoint(3, 3, atLeastFive), 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(SparseTable, BuildsFromRangesAndRepeatedValues) {
  using sparseTableTest::Minimum;
  std::array values{4, 1, 7};
  SparseTable<int, Minimum> fromRange(values);
  SparseTable<int, Minimum> repeated(4, 9);

  EXPECT_EQ(fromRange.size(), 3);
  EXPECT_EQ(fromRange.allProduct(), 1);
  EXPECT_EQ(repeated.size(), 4);
  EXPECT_EQ(repeated.allProduct(), 9);
}
// cpsc:test:end

// cpsc:test:start
TEST(SparseTable, HandlesEmptyAndSingletonTables) {
  using sparseTableTest::Minimum;
  SparseTable<int, Minimum> empty;
  SparseTable<int, Minimum> single(std::vector{7});

  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_FALSE(single.empty());
  EXPECT_EQ(single.size(), 1);
  EXPECT_EQ(single.product(0, 1), 7);
}
// cpsc:test:end
