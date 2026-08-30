/* cpsc:meta:start
snippet: dinic
cpsc:meta:end */

#include "test-support.hh"
#include "dinic.hh"

// cpsc:test:start
TEST(Dinic, FindsMaximumFlow) {
  DinicGraph<int, long long> dinic(6);
  dinic.addEdge(0, 1, 10);
  dinic.addEdge(0, 2, 10);
  dinic.addEdge(1, 2, 2);
  dinic.addEdge(1, 3, 4);
  dinic.addEdge(1, 4, 8);
  dinic.addEdge(2, 4, 9);
  dinic.addEdge(3, 5, 10);
  dinic.addEdge(4, 3, 6);
  dinic.addEdge(4, 5, 10);

  EXPECT_EQ(dinic.flow(0, 5), 19);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, SupportsParallelAndReverseEdges) {
  DinicGraph<int, int> dinic(3);
  dinic.addEdge(0, 1, 2);
  dinic.addEdge(0, 1, 3);
  dinic.addEdge(1, 0, 7);
  dinic.addEdge(1, 2, 5);

  EXPECT_EQ(dinic.flow(0, 2), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, SupportsFlowLimitAndRepeatedCalls) {
  DinicGraph<int, int> dinic(4);
  dinic.addEdge(0, 1, 4);
  dinic.addEdge(0, 2, 3);
  dinic.addEdge(1, 3, 4);
  dinic.addEdge(2, 3, 3);

  EXPECT_EQ(dinic.flow(0, 3, 5), 5);
  EXPECT_EQ(dinic.flow(0, 3), 2);
  EXPECT_EQ(dinic.flow(0, 3), 0);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, HandlesDisconnectedAndSameVertices) {
  DinicGraph<unsigned int, unsigned int> disconnected(3);
  disconnected.addEdge(0, 1, 10);
  EXPECT_EQ(disconnected.flow(0, 2), 0U);
  EXPECT_EQ(disconnected.flow(1, 1), 0U);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, HandlesSelfLoops) {
  DinicGraph<int, int> dinic(3);
  dinic.addEdge(0, 0, 100);
  dinic.addEdge(0, 1, 3);
  dinic.addEdge(1, 1, 100);
  dinic.addEdge(1, 2, 3);
  dinic.addEdge(2, 2, 100);

  EXPECT_EQ(dinic.flow(0, 2), 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, FindsMinimumCutAfterMaximumFlow) {
  DinicGraph<int, int> dinic(4);
  dinic.addEdge(0, 1, 2);
  dinic.addEdge(0, 2, 1);
  dinic.addEdge(1, 3, 1);
  dinic.addEdge(2, 3, 1);

  EXPECT_EQ(dinic.flow(0, 3), 2);
  EXPECT_EQ(dinic.minCut(0), (std::vector<bool>{true, true, false, false}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, ReturnsEdgeIdsAndExposesEdges) {
  DinicGraph<unsigned int, long long> dinic(3);
  auto first = dinic.addEdge(0, 1, 5);
  auto second = dinic.addEdge(1, 2, 3);

  EXPECT_EQ(first, 0U);
  EXPECT_EQ(second, 1U);
  EXPECT_EQ(dinic.vertexCount(), 3U);
  EXPECT_EQ(dinic.edgeCount(), 2U);

  EXPECT_EQ(dinic.flow(0, 2), 3);
  auto edge = dinic.edge(first);
  EXPECT_EQ(edge.from, 0U);
  EXPECT_EQ(edge.to, 1U);
  EXPECT_EQ(edge.capacity, 5);
  EXPECT_EQ(edge.flow, 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, SetsCapacityAndClearsFlow) {
  DinicGraph<int, int> dinic(2);
  auto edgeIndex = dinic.addEdge(0, 1, 3);

  EXPECT_EQ(dinic.flow(0, 1), 3);
  dinic.setCapacity(edgeIndex, 5);
  EXPECT_EQ(dinic.edge(edgeIndex).capacity, 5);
  EXPECT_EQ(dinic.edge(edgeIndex).flow, 3);
  EXPECT_EQ(dinic.flow(0, 1), 2);

  dinic.clearFlow();
  EXPECT_EQ(dinic.edge(edgeIndex).capacity, 5);
  EXPECT_EQ(dinic.edge(edgeIndex).flow, 0);
  EXPECT_EQ(dinic.flow(0, 1), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dinic, ExposesSelfLoopEdge) {
  DinicGraph<int, int> dinic(1);
  auto edgeIndex = dinic.addEdge(0, 0, 7);
  auto edge = dinic.edge(edgeIndex);

  EXPECT_EQ(edge.from, 0);
  EXPECT_EQ(edge.to, 0);
  EXPECT_EQ(edge.capacity, 7);
  EXPECT_EQ(edge.flow, 0);
}
// cpsc:test:end
