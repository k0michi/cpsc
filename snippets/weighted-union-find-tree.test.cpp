/* cpsc:meta:start
snippet: weighted-union-find-tree
cpsc:meta:end */

#include "test-support.hpp"
#include "weighted-union-find-tree.cpp"

// cpsc:test:start
TEST(WeightedUnionFindTree, MaintainsPotentialDifferences) {
  WeightedUnionFindTree tree(5);

  EXPECT_EQ(tree.unite(0, 1, 3), (std::tuple{true, true, std::size_t{0}}));
  EXPECT_EQ(tree.unite(1, 2, -5), (std::tuple{true, true, std::size_t{0}}));
  EXPECT_EQ(tree.diff(0, 1), std::optional{3LL});
  EXPECT_EQ(tree.diff(0, 2), std::optional{-2LL});
  EXPECT_EQ(tree.diff(2, 0), std::optional{2LL});
  EXPECT_EQ(tree.weight(2), -2);
  EXPECT_EQ(tree.size(1), 3);
  EXPECT_EQ(tree.groupCount(), 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(WeightedUnionFindTree, ReportsRedundantAndContradictoryConstraints) {
  WeightedUnionFindTree tree(3);
  tree.unite(0, 1, 4);
  tree.unite(1, 2, 7);

  EXPECT_EQ(tree.unite(0, 2, 11), (std::tuple{true, false, std::size_t{0}}));
  EXPECT_EQ(tree.unite(0, 2, 10), (std::tuple{false, false, std::size_t{0}}));
  EXPECT_EQ(tree.diff(0, 2), std::optional{11LL});
  EXPECT_EQ(tree.groupCount(), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(WeightedUnionFindTree, PreservesDifferencesWhenRootsAreSwapped) {
  WeightedUnionFindTree tree(5);
  tree.unite(0, 1, 3);
  tree.unite(1, 2, -5);
  tree.unite(3, 4, 7);

  EXPECT_EQ(tree.unite(3, 0, 10), (std::tuple{true, true, std::size_t{0}}));
  EXPECT_EQ(tree.diff(3, 0), std::optional{10LL});
  EXPECT_EQ(tree.diff(4, 2), std::optional{1LL});
  EXPECT_EQ(tree.size(4), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(WeightedUnionFindTree, HandlesDisconnectedEmptyAndClearedStates) {
  WeightedUnionFindTree<> empty;
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(empty.groupCount(), 0);

  WeightedUnionFindTree tree(2);
  EXPECT_EQ(tree.diff(0, 1), std::nullopt);
  EXPECT_EQ(tree.unite(0, 0, 0), (std::tuple{true, false, std::size_t{0}}));

  tree.unite(0, 1, 9);
  tree.clear();
  EXPECT_FALSE(tree.same(0, 1));
  EXPECT_EQ(tree.weight(0), 0);
  EXPECT_EQ(tree.weight(1), 0);
  EXPECT_EQ(tree.groupCount(), 2);
}
// cpsc:test:end
