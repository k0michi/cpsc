/* cpsc:meta:start
snippet: strongly-connected-components
cpsc:meta:end */

#include "test-support.hpp"
#include "strongly-connected-components.cpp"

// cpsc:test:start
TEST(StronglyConnectedComponents, DecomposesGraphInTopologicalOrder) {
  std::vector<std::vector<int>> graph{
      {1}, {2, 3}, {0}, {4}, {3, 5}, {}};

  auto groups = stronglyConnectedComponents<int>(graph);
  for (auto& group : groups) {
    std::sort(group.begin(), group.end());
  }

  EXPECT_EQ(groups,
            (std::vector<std::vector<int>>{{0, 1, 2}, {3, 4}, {5}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(StronglyConnectedComponents, HandlesSelfLoopsAndIsolatedVertices) {
  std::vector<std::vector<int>> graph{{0}, {}, {3}, {2}};
  auto groups = stronglyConnectedComponents<int>(graph);

  for (auto& group : groups) {
    std::sort(group.begin(), group.end());
  }
  std::sort(groups.begin(), groups.end());

  EXPECT_EQ(groups,
            (std::vector<std::vector<int>>{{0}, {1}, {2, 3}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(StronglyConnectedComponents, SupportsMemberAndTupleEdges) {
  struct E {
    int to;
  };
  std::vector<std::vector<E>> memberGraph{{{1}}, {{0}}};
  EXPECT_EQ(stronglyConnectedComponents<int>(memberGraph).size(), 1);

  std::vector<std::vector<std::pair<int, int>>> tupleGraph{
      {{1, 10}}, {{0, 20}}};
  EXPECT_EQ(stronglyConnectedComponents<int>(tupleGraph).size(), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(StronglyConnectedComponents, HandlesEmptyGraph) {
  std::vector<std::vector<int>> graph;
  EXPECT_TRUE(stronglyConnectedComponents<int>(graph).empty());
}
// cpsc:test:end
