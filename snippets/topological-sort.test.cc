/* cpsc:meta:start
snippet: topological-sort
cpsc:meta:end */

#include "test-support.hh"
#include "topological-sort.hh"

// cpsc:test:start
TEST(TopologicalSort, SortsGraphWithIntegralEdges) {
  std::vector<std::vector<int>> graph{{2}, {2, 3}, {4}, {4}, {}};
  auto order = topologicalSort<int>(graph);

  ASSERT_TRUE(order.has_value());
  std::vector<int> position(graph.size());
  for (int i = 0; i < static_cast<int>(order->size()); ++i) {
    position[(*order)[i]] = i;
  }
  for (int u = 0; u < static_cast<int>(graph.size()); ++u) {
    for (int v : graph[u]) {
      EXPECT_LT(position[u], position[v]);
    }
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(TopologicalSort, ReturnsLexicographicallySmallestOrder) {
  std::vector<std::vector<int>> graph{{3}, {3}, {4}, {4}, {}};

  EXPECT_EQ(lexicographicalTopologicalSort<int>(graph),
            std::optional(std::vector<int>{0, 1, 2, 3, 4}));
}
// cpsc:test:end

// cpsc:test:start
TEST(TopologicalSort, SupportsMemberAndTupleEdges) {
  struct Edge {
    int to;
    long long cost;
  };
  std::vector<std::vector<Edge>> memberGraph{{{1, 10}}, {{2, 20}}, {}};
  EXPECT_EQ(topologicalSort<int>(memberGraph),
            std::optional(std::vector<int>{0, 1, 2}));

  std::vector<std::vector<std::pair<int, long long>>> tupleGraph{
      {{1, 10}}, {{2, 20}}, {}};
  EXPECT_EQ(topologicalSort<int>(tupleGraph),
            std::optional(std::vector<int>{0, 1, 2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(TopologicalSort, DetectsCyclesAndHandlesEmptyGraph) {
  std::vector<std::vector<int>> cyclic{{1}, {2}, {0}};
  EXPECT_EQ(topologicalSort<int>(cyclic), std::nullopt);
  EXPECT_EQ(lexicographicalTopologicalSort<int>(cyclic), std::nullopt);

  std::vector<std::vector<int>> empty;
  EXPECT_EQ(topologicalSort<int>(empty), std::optional(std::vector<int>{}));
  EXPECT_EQ(lexicographicalTopologicalSort<int>(empty),
            std::optional(std::vector<int>{}));
}
// cpsc:test:end

// cpsc:test:start
TEST(TopologicalSort, HandlesSingleAndDisconnectedVertices) {
  std::vector<std::vector<int>> single(1);
  EXPECT_EQ(topologicalSort<int>(single), std::optional(std::vector<int>{0}));

  std::vector<std::vector<int>> disconnected(4);
  EXPECT_EQ(lexicographicalTopologicalSort<int>(disconnected),
            std::optional(std::vector<int>{0, 1, 2, 3}));
}
// cpsc:test:end
