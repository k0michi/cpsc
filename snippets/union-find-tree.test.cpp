/* cpsc:meta:start
snippet: union-find-tree
cpsc:meta:end */

#include "test-support.hpp"
#include "union-find-tree.cpp"

// cpsc:test:start
TEST(UnionFindTree, UnitesAndQueriesComponents) {
  UnionFindTree dsu(6);

  EXPECT_EQ(dsu.size(), 6);
  EXPECT_EQ(dsu.groupCount(), 6);
  EXPECT_TRUE(dsu.unite(0, 1));
  EXPECT_TRUE(dsu.unite(1, 2));
  EXPECT_FALSE(dsu.unite(0, 2));
  EXPECT_TRUE(dsu.same(0, 2));
  EXPECT_FALSE(dsu.same(0, 3));
  EXPECT_EQ(dsu.size(1), 3);
  EXPECT_EQ(dsu.groupCount(), 4);
  EXPECT_EQ(dsu.leader(0), dsu.leader(2));
}
// cpsc:test:end

// cpsc:test:start
TEST(UnionFindTree, EnumeratesGroups) {
  UnionFindTree dsu(5);
  dsu.unite(0, 1);
  dsu.unite(3, 4);
  dsu.unite(1, 4);

  auto groups = dsu.groups();
  std::sort(groups.begin(), groups.end());

  EXPECT_EQ(groups, (std::vector<std::vector<std::size_t>>{{0, 1, 3, 4}, {2}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(UnionFindTree, ClearsWithoutChangingElementCount) {
  UnionFindTree dsu(4);
  dsu.unite(0, 1);
  dsu.unite(2, 3);

  dsu.clear();

  EXPECT_EQ(dsu.size(), 4);
  EXPECT_EQ(dsu.groupCount(), 4);
  EXPECT_FALSE(dsu.same(0, 1));
  EXPECT_EQ(dsu.size(0), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(UnionFindTree, ResizesAndHandlesEmptyState) {
  UnionFindTree dsu;
  EXPECT_TRUE(dsu.empty());
  EXPECT_EQ(dsu.groupCount(), 0);

  dsu.resize(3);
  EXPECT_FALSE(dsu.empty());
  EXPECT_EQ(dsu.size(), 3);
  EXPECT_EQ(dsu.groupCount(), 3);

  dsu.resize(0);
  EXPECT_TRUE(dsu.empty());
}
// cpsc:test:end

// cpsc:test:start
TEST(UnionFindTree, HandlesSingleElement) {
  UnionFindTree dsu(1);

  EXPECT_FALSE(dsu.empty());
  EXPECT_EQ(dsu.leader(0), 0);
  EXPECT_EQ(dsu.size(0), 1);
  EXPECT_EQ(dsu.groupCount(), 1);
  EXPECT_TRUE(dsu.same(0, 0));
  EXPECT_FALSE(dsu.unite(0, 0));
  EXPECT_EQ(dsu.groups(), (std::vector<std::vector<std::size_t>>{{0}}));
}
// cpsc:test:end
