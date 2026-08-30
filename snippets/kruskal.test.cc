/* cpsc:meta:start
snippet: kruskal
cpsc:meta:end */

#include "test-support.hh"
#include "kruskal.hh"

// cpsc:test:start
TEST(Kruskal, FindsMinimumSpanningTreeWithTupleEdges) {
  using Edge = std::tuple<int, int, long long>;
  std::vector<Edge> edges{
      {0, 1, 4}, {0, 2, 1}, {1, 2, 2}, {1, 3, 1}, {2, 3, 5}};
  auto result = kruskal<int, long long>(4, edges);

  EXPECT_TRUE(result.isConnected);
  EXPECT_EQ(result.totalWeight, 4);
  EXPECT_EQ(result.edges.size(), 3U);
}
// cpsc:test:end

// cpsc:test:start
TEST(Kruskal, SupportsMemberEdgesAndNegativeCosts) {
  struct Edge {
    unsigned int from;
    unsigned int to;
    int cost;
  };
  std::vector<Edge> edges{{0, 1, -2}, {1, 2, 3}, {0, 2, 10}};
  auto result = kruskal<unsigned int, long long>(3, edges);

  EXPECT_TRUE(result.isConnected);
  EXPECT_EQ(result.totalWeight, 1);
  EXPECT_EQ(result.edges[0].cost, -2);
}
// cpsc:test:end

// cpsc:test:start
TEST(Kruskal, ReturnsMinimumSpanningForestForDisconnectedGraph) {
  using Edge = std::tuple<int, int, int>;
  std::vector<Edge> edges{{0, 1, 2}, {2, 3, 3}};
  auto result = kruskal<int, int>(5, edges);

  EXPECT_FALSE(result.isConnected);
  EXPECT_EQ(result.totalWeight, 5);
  EXPECT_EQ(result.edges.size(), 2U);
}
// cpsc:test:end

// cpsc:test:start
TEST(Kruskal, HandlesEmptyAndSingleVertexGraphs) {
  using Edge = std::tuple<int, int, int>;
  std::vector<Edge> edges;

  EXPECT_TRUE(kruskal<int, int>(0, edges).isConnected);
  EXPECT_TRUE(kruskal<int, int>(1, edges).isConnected);
  EXPECT_EQ(kruskal<int, int>(1, edges).totalWeight, 0);
}
// cpsc:test:end

// cpsc:test:start
TEST(Kruskal, IgnoresSelfLoopsAndChoosesCheapestParallelEdge) {
  using Edge = std::tuple<int, int, int>;
  std::vector<Edge> edges{
      {0, 0, -100}, {0, 1, 5}, {0, 1, 2}, {1, 2, 3}, {0, 2, 10}};
  auto result = kruskal<int, int>(3, edges);

  EXPECT_TRUE(result.isConnected);
  EXPECT_EQ(result.totalWeight, 5);
  EXPECT_EQ(result.edges.size(), 2U);
}
// cpsc:test:end

#ifndef NDEBUG
// cpsc:test:start
TEST(Kruskal, RejectsNegativeVertexCountAndInvalidVertices) {
  using Edge = std::tuple<int, int, int>;
  EXPECT_DEATH((void)kruskal<int, int>(-1, std::vector<Edge>{}), "");
  EXPECT_DEATH((void)kruskal<int, int>(2, std::vector<Edge>{{-1, 1, 1}}), "");
  EXPECT_DEATH((void)kruskal<int, int>(2, std::vector<Edge>{{0, 2, 1}}), "");
}
// cpsc:test:end
#endif
