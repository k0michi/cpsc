/* cpsc:meta:start
snippet: bfs
cpsc:meta:end */

#include "test-support.hh"
#include "bfs.hh"

// cpsc:test:start
TEST(Bfs, FindsShortestEdgeCounts) {
  std::vector<std::vector<int>> graph{{1, 2}, {3}, {3, 4}, {5}, {}, {}};
  auto result = bfs(graph, 0);
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<int>>{0, 1, 1, 2, 2, 3}));
  EXPECT_EQ(result.pathTo(5), (std::vector<int>{0, 1, 3, 5}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, MarksUnreachableVertices) {
  std::vector<std::vector<int>> graph{{1}, {}, {3}, {}};
  auto result = bfs(graph, 2);
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{
                                 std::nullopt, std::nullopt, 0, 1}));
  EXPECT_FALSE(result.reachable(0));
  EXPECT_TRUE(result.reachable(3));
  EXPECT_TRUE(result.pathTo(0).empty());
  EXPECT_EQ(result.pathTo(2), (std::vector<int>{2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsEdgesWithoutCost) {
  struct Edge {
    int to;
  };
  std::vector<std::vector<Edge>> memberGraph{{{1}}, {{2}}, {}};
  EXPECT_EQ(bfs(memberGraph, 0).distance,
            (std::vector<std::optional<int>>{0, 1, 2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, RunsZeroOneBfsWithTupleEdges) {
  using Edge = std::pair<int, bool>;
  std::vector<std::vector<Edge>> graph{
      {{1, true}, {2, false}}, {{3, true}}, {{1, false}, {3, true}}, {}};
  auto result = bfs(graph, 0);
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 0, 1}));
  EXPECT_EQ(result.pathTo(3), (std::vector<int>{0, 2, 3}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, RunsZeroOneBfsWithMemberEdges) {
  struct Edge {
    int to;
    bool weight;
  };
  std::vector<std::vector<Edge>> graph{
      {{1, true}, {2, false}}, {{3, true}}, {{1, false}}, {}};
  auto result = bfs(graph, 0);
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 0, 1}));
  EXPECT_EQ(result.previous[1], 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsUnsignedVertexAndDistanceType) {
  std::vector<std::vector<unsigned int>> graph{{1}, {2}, {}};
  auto result = bfs(graph, 0U);
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<unsigned int>>{0U, 1U, 2U}));
  EXPECT_EQ(result.pathTo(2U), (std::vector<unsigned int>{0U, 1U, 2U}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsWeightMemberAndConvertsCostToBool) {
  struct Edge {
    int to;
    int weight;
  };
  std::vector<std::vector<Edge>> graph{
      {{1, 1}, {2, 0}}, {{3, 2}}, {{1, 0}}, {}};
  auto result = bfs(graph, 0);
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 0, 1}));
}
// cpsc:test:end
