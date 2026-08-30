/* cpsc:meta:start
snippet: dijkstra
cpsc:meta:end */

#include "test-support.hh"
#include "dijkstra.hh"

// cpsc:test:start
TEST(Dijkstra, FindsShortestDistances) {
  using Edge = std::pair<int, long long>;
  std::vector<std::vector<Edge>> graph{
      {{1, 4}, {2, 1}}, {{3, 1}}, {{1, 2}, {3, 5}}, {}, {}};
  auto result = dijkstra<int, long long>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<long long>>{0, 3, 1, 4, std::nullopt}));
  EXPECT_EQ(result.pathTo(3), (std::vector<int>{0, 2, 1, 3}));
  EXPECT_FALSE(result.reachable(4));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dijkstra, SupportsMemberEdgesAndZeroCost) {
  struct Edge {
    int to;
    int cost;
  };
  std::vector<std::vector<Edge>> graph{{{1, 0}, {2, 5}}, {{2, 2}}, {}};
  auto result = dijkstra<int, int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 2}));
  EXPECT_EQ(result.previous[0], std::nullopt);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dijkstra, AvoidsOverflowAtInfinity) {
  using Edge = std::pair<int, unsigned int>;
  const auto max = std::numeric_limits<unsigned int>::max();
  std::vector<std::vector<Edge>> graph{{{1, max - 1}}, {{2, 2}}, {}};
  auto result = dijkstra<int, unsigned int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance, (std::vector<std::optional<unsigned int>>{
                                 0, max - 1, std::nullopt}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Dijkstra, SupportsMultipleStartsFromRange) {
  using Edge = std::pair<unsigned int, int>;
  std::vector<std::vector<Edge>> graph{
      {{1, 4}}, {{2, 3}}, {{3, 2}}, {{4, 1}}, {}};
  std::vector<unsigned int> starts{0, 4};
  auto result = dijkstra<unsigned int, int>(graph, starts);

  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 4, 7, 9, 0}));
  EXPECT_EQ(result.pathTo(3), (std::vector<unsigned int>{0, 1, 2, 3}));
  EXPECT_EQ(result.previous[4], std::nullopt);
}
// cpsc:test:end

// cpsc:test:start
TEST(Dijkstra, SupportsGraphReturningEdgeVectors) {
  struct Edge {
    int to;
    double cost;
  };
  struct GeneratedGraph {
    int count;

    [[nodiscard]] std::size_t size() const {
      return static_cast<std::size_t>(count);
    }

    [[nodiscard]] std::vector<Edge> operator[](int vertex) const {
      if (vertex + 1 < count) {
        return {{vertex + 1, 0.5}};
      }
      return {};
    }
  };

  GeneratedGraph graph{.count = 3};
  auto result = dijkstra<int, double>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<double>>{0.0, 0.5, 1.0}));
  EXPECT_EQ(result.pathTo(2), (std::vector<int>{0, 1, 2}));
}
// cpsc:test:end
