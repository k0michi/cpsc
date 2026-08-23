/* cpsc:meta:start
snippet: bound-helpers
cpsc:meta:end */

#include "test-support.hpp"
#include "bound-helpers.cpp"

// cpsc:test:start
TEST(BoundHelpers, FindsAllBoundsInSet) {
  std::set<int> values{2, 4, 6};

  EXPECT_EQ(*::floor(values, 4), 4);
  EXPECT_EQ(*::floor(values, 5), 4);
  EXPECT_EQ(::floor(values, 1), values.end());
  EXPECT_EQ(*::lower(values, 4), 2);
  EXPECT_EQ(::lower(values, 2), values.end());
  EXPECT_EQ(*::ceil(values, 4), 4);
  EXPECT_EQ(*::ceil(values, 5), 6);
  EXPECT_EQ(::ceil(values, 7), values.end());
  EXPECT_EQ(*::higher(values, 4), 6);
  EXPECT_EQ(::higher(values, 6), values.end());
}
// cpsc:test:end

// cpsc:test:start
TEST(BoundHelpers, HandlesDuplicatesAndMaps) {
  std::multiset<int> values{2, 4, 4, 6};
  EXPECT_EQ(*::floor(values, 4), 4);
  EXPECT_EQ(*::lower(values, 4), 2);
  EXPECT_EQ(*::ceil(values, 4), 4);
  EXPECT_EQ(*::higher(values, 4), 6);

  std::map<int, char> mapping{{2, 'a'}, {5, 'b'}};
  EXPECT_EQ(::floor(mapping, 4)->first, 2);
  EXPECT_EQ(::ceil(mapping, 4)->first, 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(BoundHelpers, FindsAllBoundsInSortedRange) {
  std::vector<int> values{2, 4, 4, 6};
  auto first = values.begin();
  auto last = values.end();

  EXPECT_EQ(*::floor(first, last, 4), 4);
  EXPECT_EQ(*::lower(first, last, 4), 2);
  EXPECT_EQ(*::ceil(first, last, 4), 4);
  EXPECT_EQ(*::higher(first, last, 4), 6);
  EXPECT_EQ(::floor(first, last, 1), last);
  EXPECT_EQ(::lower(first, last, 2), last);
  EXPECT_EQ(::ceil(first, last, 7), last);
  EXPECT_EQ(::higher(first, last, 6), last);
}
// cpsc:test:end

// cpsc:test:start
TEST(BoundHelpers, SupportsCustomOrdering) {
  std::vector<int> values{9, 7, 5, 3};
  auto comp = std::greater{};

  EXPECT_EQ(*::floor(values.begin(), values.end(), 6, comp), 7);
  EXPECT_EQ(*::lower(values.begin(), values.end(), 7, comp), 9);
  EXPECT_EQ(*::ceil(values.begin(), values.end(), 6, comp), 5);
  EXPECT_EQ(*::higher(values.begin(), values.end(), 7, comp), 5);
}
// cpsc:test:end
