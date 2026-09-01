/* cpsc:meta:start
snippet: lazy-segment-tree
cpsc:meta:end */

#include "test-support.hh"
#include "lazy-segment-tree.hh"

// cpsc:test:start
TEST(LazySegmentTree, AppliesRangeUpdatesAndQueriesProducts) {
  using T = std::pair<long long, std::size_t>;
  auto op = [](T a, T b) { return T{a.first + b.first, a.second + b.second}; };
  auto mapping = [](long long f, T value) {
    return T{value.first + f * static_cast<long long>(value.second),
             value.second};
  };
  auto composition = [](long long f, long long g) { return f + g; };
  std::vector<T> values{{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
  LazySegmentTree<T, long long, decltype(op), decltype(mapping),
                  decltype(composition)>
      tree(values, T{0, 0}, 0, op, mapping, composition);

  EXPECT_EQ(tree.product(0, 5).first, 15);
  tree.apply(1, 4, 10);
  EXPECT_EQ(tree.product(1, 4).first, 39);
  EXPECT_EQ(tree.allProduct().first, 45);
  EXPECT_EQ(tree.get(2).first, 13);

  tree.apply(0, -1);
  tree.set(4, T{20, 1});
  EXPECT_EQ(tree[0].first, 0);
  EXPECT_EQ(tree.allProduct().first, 59);
}
// cpsc:test:end

// cpsc:test:start
TEST(LazySegmentTree, ComposesPendingActionsInOrder) {
  using T = std::pair<long long, std::size_t>;
  using F = std::pair<long long, long long>;
  auto op = [](T a, T b) { return T{a.first + b.first, a.second + b.second}; };
  auto mapping = [](F f, T value) {
    return T{f.first * value.first +
                 f.second * static_cast<long long>(value.second),
             value.second};
  };
  auto composition = [](F f, F g) {
    return F{f.first * g.first, f.first * g.second + f.second};
  };
  std::vector<T> values{{1, 1}, {2, 1}, {3, 1}, {4, 1}};
  LazySegmentTree<T, F, decltype(op), decltype(mapping), decltype(composition)>
      tree(values, T{0, 0}, F{1, 0}, op, mapping, composition);

  tree.apply(0, 4, F{2, 1});
  tree.apply(0, 4, F{1, 3});

  EXPECT_EQ(tree.allProduct().first, 36);
  EXPECT_EQ(tree.get(0).first, 6);
  EXPECT_EQ(tree.get(3).first, 12);
}
// cpsc:test:end

// cpsc:test:start
TEST(LazySegmentTree, FindsPartitionPointsAfterLazyUpdates) {
  using T = std::pair<long long, std::size_t>;
  auto op = [](T a, T b) { return T{a.first + b.first, a.second + b.second}; };
  auto mapping = [](long long f, T value) {
    return T{value.first + f * static_cast<long long>(value.second),
             value.second};
  };
  auto composition = [](long long f, long long g) { return f + g; };
  std::vector<T> values{{2, 1}, {1, 1}, {3, 1}, {4, 1}, {2, 1}};
  LazySegmentTree<T, long long, decltype(op), decltype(mapping),
                  decltype(composition)>
      tree(values, T{0, 0}, 0, op, mapping, composition);

  tree.apply(1, 4, 1);
  auto atMostEight = [](T value) { return value.first <= 8; };
  auto atMostSeven = [](T value) { return value.first <= 7; };

  EXPECT_EQ(tree.partitionPoint(0, atMostEight), 3);
  EXPECT_EQ(tree.partitionPoint(1, 4, atMostSeven), 3);
  EXPECT_EQ(tree.partitionPointReverse(5, atMostSeven), 3);
  EXPECT_EQ(tree.partitionPointReverse(1, 4, atMostSeven), 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(LazySegmentTree, HandlesEmptySingletonAndClear) {
  using T = std::pair<long long, std::size_t>;
  auto op = [](T a, T b) { return T{a.first + b.first, a.second + b.second}; };
  auto mapping = [](long long f, T value) {
    return T{value.first + f * static_cast<long long>(value.second),
             value.second};
  };
  auto composition = [](long long f, long long g) { return f + g; };
  using Tree = LazySegmentTree<T, long long, decltype(op), decltype(mapping),
                               decltype(composition)>;

  Tree empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.product(0, 0), (T{0, 0}));
  EXPECT_EQ(empty.allProduct(), (T{0, 0}));

  Tree tree(std::vector<T>{{7, 1}}, T{0, 0}, 0, op, mapping, composition);
  tree.apply(0, 1, 5);
  EXPECT_EQ(tree.get(0), (T{12, 1}));
  tree.clear();
  EXPECT_EQ(tree.get(0), (T{0, 0}));
  EXPECT_EQ(tree.allProduct(), (T{0, 0}));
}
// cpsc:test:end

// cpsc:test:start
TEST(LazySegmentTree, DefaultConstructsCapturelessOperations) {
  auto op = [](long long a, long long b) { return std::max(a, b); };
  auto mapping = [](long long f, long long value) { return f + value; };
  auto composition = [](long long f, long long g) { return f + g; };
  using Tree = LazySegmentTree<long long, long long, decltype(op),
                               decltype(mapping), decltype(composition)>;

  Tree tree(4);
  tree.apply(1, 4, 3);

  EXPECT_EQ(tree.product(0, 4), 3);
  EXPECT_EQ(tree.get(0), 0);
  EXPECT_EQ(tree.get(2), 3);

  Tree initialized(3, 5);
  EXPECT_EQ(initialized.product(0, 3), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(LazySegmentTree, DeducesTypesFromSizeAndRange) {
  auto op = [](int a, int b) { return std::max(a, b); };
  auto mapping = [](int f, int value) { return f + value; };
  auto composition = [](int f, int g) { return f + g; };

  LazySegmentTree sized(std::size_t{3}, 0, 0, 0, op, mapping, composition);
  sized.apply(0, 2, 4);
  EXPECT_EQ(sized.allProduct(), 4);

  std::deque<int> values{1, 5, 2};
  LazySegmentTree ranged(values, 0, 0, op, mapping, composition);
  ranged.apply(2, 3, 6);
  EXPECT_EQ(ranged.product(0, 3), 8);
}
// cpsc:test:end
