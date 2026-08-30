/* cpsc:meta:start
snippet: dfs
cpsc:meta:end */

#include "test-support.hh"
#include "dfs.hh"

// cpsc:test:start
TEST(Dfs, VisitsReachableVerticesInDepthFirstOrder) {
  std::vector<std::vector<int>> graph{{1, 2}, {3}, {4}, {2}, {}, {0}};
  auto result = dfs<int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.order, (std::vector<int>{0, 1, 3, 2, 4}));
  EXPECT_EQ(result.pathTo(4), (std::vector<int>{0, 1, 3, 2, 4}));
  EXPECT_FALSE(result.reachable(5));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dfs, SupportsCyclesAndDisconnectedVertices) {
  std::vector<std::vector<int>> graph{{1}, {2}, {0}, {4}, {}};
  auto result = dfs<int>(graph, std::initializer_list{3});
  EXPECT_EQ(result.order, (std::vector<int>{3, 4}));
  EXPECT_EQ(result.previous[3], std::nullopt);
  EXPECT_TRUE(result.pathTo(0).empty());
}
// cpsc:test:end

// cpsc:test:start
TEST(Dfs, SupportsMemberAndTupleEdges) {
  struct Edge {
    int to;
  };
  std::vector<std::vector<Edge>> memberGraph{{{1}}, {{2}}, {}};
  EXPECT_EQ(dfs<int>(memberGraph, std::initializer_list{0}).order,
            (std::vector<int>{0, 1, 2}));

  std::vector<std::vector<std::pair<int, int>>> tupleGraph{
      {{1, 10}}, {{2, 20}}, {}};
  EXPECT_EQ(dfs<int>(tupleGraph, std::initializer_list{0}).order,
            (std::vector<int>{0, 1, 2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dfs, SupportsMultipleStartsFromRange) {
  std::vector<std::vector<int>> graph{{1}, {}, {3}, {}, {}};
  std::vector<unsigned int> starts{2, 0};
  auto result = dfs<unsigned int>(graph, starts);

  EXPECT_EQ(result.order, (std::vector<unsigned int>{2, 3, 0, 1}));
  EXPECT_EQ(result.previous[2], std::nullopt);
  EXPECT_EQ(result.previous[0], std::nullopt);
  EXPECT_FALSE(result.reachable(4));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dfs, SupportsGraphReturningNeighborVectors) {
  struct GeneratedGraph {
    int vertexCount;

    [[nodiscard]] std::size_t size() const {
      return static_cast<std::size_t>(vertexCount);
    }

    [[nodiscard]] std::vector<int> operator[](int vertex) const {
      std::vector<int> neighbors;
      if (vertex + 1 < vertexCount) {
        neighbors.push_back(vertex + 1);
      }
      return neighbors;
    }
  };

  GeneratedGraph graph{.vertexCount = 4};
  auto result = dfs<int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.order, (std::vector<int>{0, 1, 2, 3}));
  EXPECT_EQ(result.pathTo(3), (std::vector<int>{0, 1, 2, 3}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dfs, SupportsTreeDpUsingOrderAndPrevious) {
  std::vector<std::vector<int>> tree{{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
  auto result = dfs<int>(tree, std::initializer_list{0});

  std::vector<int> subtreeSize(tree.size(), 1);
  for (auto it = result.order.rbegin(); it != result.order.rend(); ++it) {
    int vertex = *it;
    if (result.previous[vertex].has_value()) {
      subtreeSize[*result.previous[vertex]] += subtreeSize[vertex];
    }
  }

  EXPECT_EQ(subtreeSize, (std::vector<int>{6, 3, 2, 1, 1, 1}));
}
// cpsc:test:end
