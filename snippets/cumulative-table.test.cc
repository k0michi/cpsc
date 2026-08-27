/* cpsc:meta:start
snippet: cumulative-table
cpsc:meta:end */

#include "test-support.hh"
#include "cumulative-table.hh"

namespace cumulativeTableTest {
using Permutation = std::array<int, 3>;

struct Compose {
  Permutation operator()(const Permutation &left,
                         const Permutation &right) const {
    Permutation result{};
    for (std::size_t i = 0; i < result.size(); ++i) {
      result[i] = left[right[i]];
    }
    return result;
  }
};

struct Inverse {
  Permutation operator()(const Permutation &value) const {
    Permutation result{};
    for (std::size_t i = 0; i < result.size(); ++i) {
      result[value[i]] = static_cast<int>(i);
    }
    return result;
  }
};
} // namespace cumulativeTableTest

// cpsc:test:start
TEST(CumulativeTable, ComputesAdditiveRangeProducts) {
  CumulativeTable product(std::vector<long long>{2, -1, 4, 3});

  EXPECT_EQ(product.product(0, 0), 0);
  EXPECT_EQ(product.product(0, 3), 5);
  EXPECT_EQ(product.product(1, 4), 6);
  EXPECT_EQ(product.product(2, 2), 0);
  EXPECT_EQ(product.allProduct(), 8);
  EXPECT_EQ(product.get(2), 4);
  EXPECT_EQ(product[3], 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(CumulativeTable, PreservesOrderForNoncommutativeGroups) {
  using namespace cumulativeTableTest;
  Permutation unit{0, 1, 2};
  Permutation first{1, 0, 2};
  Permutation second{0, 2, 1};
  Permutation third{2, 1, 0};
  std::vector values{first, second, third};
  CumulativeTable<Permutation, Compose, Inverse> product(values, unit);
  Compose compose;

  EXPECT_EQ(product.product(1, 3), compose(second, third));
  EXPECT_EQ(product.product(0, 3), compose(compose(first, second), third));
}
// cpsc:test:end

// cpsc:test:start
TEST(CumulativeTable, FindsPartitionPointsInBothDirections) {
  CumulativeTable product(std::vector<long long>{2, 1, 3, 2, 4});
  auto atMostSix = [](long long value) { return value <= 6; };

  EXPECT_EQ(product.partitionPoint(1, atMostSix), 4);
  EXPECT_EQ(product.partitionPoint(1, 4, atMostSix), 4);
  EXPECT_EQ(product.partitionPoint(2, 2, atMostSix), 2);

  EXPECT_EQ(product.partitionPointReverse(5, atMostSix), 3);
  EXPECT_EQ(product.partitionPointReverse(1, 4, atMostSix), 1);
  EXPECT_EQ(product.partitionPointReverse(2, 2, atMostSix), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(CumulativeTable, BuildsFromRangesAndRepeatedUnits) {
  std::array values{1, 2, 3};
  CumulativeTable<int> fromRange(values);
  CumulativeTable<long long> units(4);

  EXPECT_EQ(fromRange.allProduct(), 6);
  EXPECT_EQ(fromRange.size(), 3);
  EXPECT_EQ(units.allProduct(), 0);
  EXPECT_EQ(units.product(0, units.size()), 0);
}
// cpsc:test:end

// cpsc:test:start
TEST(CumulativeTable, HandlesEmptyAndSingletonSequences) {
  CumulativeTable<long long> empty;
  CumulativeTable single(std::vector<long long>{7});

  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(empty.allProduct(), 0);
  EXPECT_EQ(empty.product(0, 0), 0);

  EXPECT_FALSE(single.empty());
  EXPECT_EQ(single.size(), 1);
  EXPECT_EQ(single.product(0, 1), 7);
}
// cpsc:test:end
