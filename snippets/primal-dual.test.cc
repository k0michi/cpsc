/* cpsc:meta:start
snippet: primal-dual
cpsc:meta:end */

#include "test-support.hh"
#include "primal-dual.hh"

// cpsc:test:start
TEST(PrimalDual, FindsMinimumCostFlow) {
  PrimalDualGraph<int, long long, long long> graph(4);
  graph.addEdge(0, 1, 2, 1);
  graph.addEdge(0, 2, 1, 5);
  graph.addEdge(1, 2, 1, 1);
  graph.addEdge(1, 3, 1, 3);
  graph.addEdge(2, 3, 2, 1);
  EXPECT_EQ(graph.flow(0, 3), (std::pair<long long, long long>{3, 13}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, ReturnsSlopeBreakpoints) {
  PrimalDualGraph<unsigned int, int, long long> graph(4);
  graph.addEdge(0, 1, 2, 1);
  graph.addEdge(0, 2, 1, 5);
  graph.addEdge(1, 2, 1, 1);
  graph.addEdge(1, 3, 1, 3);
  graph.addEdge(2, 3, 2, 1);
  EXPECT_EQ(graph.slope(0, 3), (std::vector<std::pair<int, long long>>{
                                   {0, 0}, {1, 3}, {2, 7}, {3, 13}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, MergesEqualSlopeSegmentsAndRespectsLimit) {
  PrimalDualGraph<int, int, int> graph(4);
  graph.addEdge(0, 1, 1, 1);
  graph.addEdge(0, 2, 1, 1);
  graph.addEdge(1, 3, 1, 1);
  graph.addEdge(2, 3, 1, 1);
  EXPECT_EQ(graph.slope(0, 3, 2),
            (std::vector<std::pair<int, int>>{{0, 0}, {2, 4}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, MatchesDinicStyleInterfaces) {
  PrimalDualGraph<unsigned int, int, long long> graph(3);
  auto selfLoop = graph.addEdge(0, 0, 7, 2);
  auto pathEdge = graph.addEdge(0, 1, 3, 4);
  graph.addEdge(1, 2, 3, 5);

  EXPECT_EQ(graph.vertexCount(), 3U);
  EXPECT_EQ(graph.edgeCount(), 3U);
  EXPECT_EQ(graph.flow(0, 2), (std::pair<int, long long>{3, 27}));
  EXPECT_EQ(graph.edge(selfLoop).capacity, 7);
  EXPECT_EQ(graph.edge(selfLoop).flow, 0);
  EXPECT_EQ(graph.edge(pathEdge).flow, 3);
  EXPECT_EQ(graph.edge(pathEdge).cost, 4);
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, SetsCapacityAndClearsFlow) {
  PrimalDualGraph<int, int, int> graph(2);
  auto edgeIndex = graph.addEdge(0, 1, 3, 2);
  EXPECT_EQ(graph.flow(0, 1), (std::pair<int, int>{3, 6}));
  graph.setCapacity(edgeIndex, 5);
  EXPECT_EQ(graph.edge(edgeIndex).capacity, 5);
  EXPECT_EQ(graph.edge(edgeIndex).flow, 3);

  graph.clearFlow();
  EXPECT_EQ(graph.edge(edgeIndex).flow, 0);
  EXPECT_EQ(graph.flow(0, 1), (std::pair<int, int>{5, 10}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, ReturnsPartialFlowWhenSinkIsUnreachable) {
  PrimalDualGraph<int, int, int> graph(4);
  graph.addEdge(0, 1, 2, 3);
  graph.addEdge(1, 3, 1, 4);
  EXPECT_EQ(graph.flow(0, 3, 5), (std::pair<int, int>{1, 7}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, SupportsConsecutiveFlowAndSlopeCalls) {
  PrimalDualGraph<int, int, int> graph(4);
  graph.addEdge(0, 1, 2, 1);
  graph.addEdge(0, 2, 1, 5);
  graph.addEdge(1, 2, 1, 1);
  graph.addEdge(1, 3, 1, 3);
  graph.addEdge(2, 3, 2, 1);

  EXPECT_EQ(graph.flow(0, 3, 1), (std::pair<int, int>{1, 3}));
  EXPECT_EQ(graph.slope(0, 3),
            (std::vector<std::pair<int, int>>{{0, 0}, {1, 4}, {2, 10}}));
  EXPECT_EQ(graph.flow(0, 3), (std::pair<int, int>{0, 0}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, ClearFlowResetsPotentialForRecalculation) {
  PrimalDualGraph<int, int, int> graph(4);
  graph.addEdge(0, 1, 1, 1);
  graph.addEdge(1, 3, 1, 2);
  graph.addEdge(0, 2, 1, 4);
  graph.addEdge(2, 3, 1, 1);

  EXPECT_EQ(graph.flow(0, 3), (std::pair<int, int>{2, 8}));
  graph.clearFlow();
  EXPECT_EQ(graph.flow(0, 3), (std::pair<int, int>{2, 8}));
}
// cpsc:test:end

#ifndef NDEBUG
// cpsc:test:start
TEST(PrimalDual, RejectsNegativeVertexIndices) {
  PrimalDualGraph<int, int, int> graph(2);
  EXPECT_DEATH(graph.addEdge(-1, 1, 1, 1), "");
  EXPECT_DEATH(graph.addEdge(0, -1, 1, 1), "");
  EXPECT_DEATH(graph.flow(-1, 1), "");
  EXPECT_DEATH(graph.flow(0, -1), "");
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalDual, RejectsNegativeVertexCount) {
  EXPECT_DEATH((PrimalDualGraph<int, int, int>(-1)), "");
}
// cpsc:test:end
#endif
