/* cpsc:meta:start
snippet: rollback-union-find-tree
cpsc:meta:end */

#include "test-support.hh"
#include "rollback-union-find-tree.hh"

// cpsc:test:start
TEST(RollbackUnionFindTree, UnitesAndUndoesComponents) {
  RollbackUnionFindTree dsu(5);

  EXPECT_EQ(dsu.unite(0, 1), (std::pair{true, std::size_t{0}}));
  EXPECT_EQ(dsu.unite(1, 2), (std::pair{true, std::size_t{0}}));
  EXPECT_TRUE(dsu.same(0, 2));
  EXPECT_EQ(dsu.size(1), 3);
  EXPECT_EQ(dsu.groupCount(), 3);

  dsu.undo();
  EXPECT_TRUE(dsu.same(0, 1));
  EXPECT_FALSE(dsu.same(0, 2));
  EXPECT_EQ(dsu.size(0), 2);
  EXPECT_EQ(dsu.groupCount(), 4);

  dsu.undo();
  EXPECT_FALSE(dsu.same(0, 1));
  EXPECT_EQ(dsu.groupCount(), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(RollbackUnionFindTree, CountsRedundantUnionAsOneOperation) {
  RollbackUnionFindTree dsu(3);
  dsu.unite(0, 1);
  auto beforeRedundant = dsu.snapshot();

  EXPECT_EQ(dsu.unite(1, 0), (std::pair{false, dsu.leader(0)}));
  EXPECT_EQ(dsu.snapshot(), beforeRedundant + 1);

  dsu.undo();
  EXPECT_EQ(dsu.snapshot(), beforeRedundant);
  EXPECT_TRUE(dsu.same(0, 1));
  EXPECT_EQ(dsu.groupCount(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(RollbackUnionFindTree, RollsBackToSnapshots) {
  RollbackUnionFindTree dsu(6);
  auto initial = dsu.snapshot();
  dsu.unite(0, 1);
  dsu.unite(2, 3);
  auto twoPairs = dsu.snapshot();
  dsu.unite(1, 2);
  dsu.unite(4, 5);

  dsu.rollback(twoPairs);
  EXPECT_TRUE(dsu.same(0, 1));
  EXPECT_TRUE(dsu.same(2, 3));
  EXPECT_FALSE(dsu.same(0, 2));
  EXPECT_FALSE(dsu.same(4, 5));
  EXPECT_EQ(dsu.groupCount(), 4);

  dsu.rollback(initial);
  EXPECT_EQ(dsu.groupCount(), 6);
  for (std::size_t i = 0; i < dsu.size(); ++i) {
    EXPECT_EQ(dsu.size(i), 1);
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(RollbackUnionFindTree, RestoresUnionBySizeSwaps) {
  RollbackUnionFindTree dsu(5);
  dsu.unite(0, 1);
  dsu.unite(0, 2);
  auto before = dsu.snapshot();
  auto [merged, leader] = dsu.unite(3, 0);

  EXPECT_TRUE(merged);
  EXPECT_EQ(leader, dsu.leader(0));
  EXPECT_EQ(dsu.size(3), 4);

  dsu.rollback(before);
  EXPECT_EQ(dsu.size(0), 3);
  EXPECT_EQ(dsu.size(3), 1);
  EXPECT_FALSE(dsu.same(0, 3));
}
// cpsc:test:end

// cpsc:test:start
TEST(RollbackUnionFindTree, ClearsResizesAndHandlesEmptyState) {
  RollbackUnionFindTree dsu;
  EXPECT_TRUE(dsu.empty());
  EXPECT_EQ(dsu.snapshot(), 0);

  dsu.resize(3);
  dsu.unite(0, 1);
  dsu.clear();
  EXPECT_EQ(dsu.size(), 3);
  EXPECT_EQ(dsu.groupCount(), 3);
  EXPECT_EQ(dsu.snapshot(), 0);
  EXPECT_FALSE(dsu.same(0, 1));

  dsu.resize(1);
  EXPECT_FALSE(dsu.empty());
  EXPECT_EQ(dsu.unite(0, 0), (std::pair{false, std::size_t{0}}));
  dsu.undo();
  EXPECT_EQ(dsu.groupCount(), 1);
}
// cpsc:test:end
