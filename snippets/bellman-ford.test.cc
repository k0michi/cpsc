/* cpsc:meta:start
snippet: bellman-ford
cpsc:meta:end */

#include "test-support.hh"
#include "bellman-ford.hh"

// cpsc:test:start
TEST(BellmanFord, FindsShortestDistancesAndPathWithNegativeEdges) {
  using Edge = std::pair<int, long long>;
  std::vector<std::vector<Edge>> graph{
      {{1, 4}, {2, 5}}, {{2, -2}, {3, 4}}, {{3, 1}}, {}, {}};
  auto result = bellmanFord<int, long long>(graph, std::initializer_list{0});

  EXPECT_EQ(result.distance,
            (std::vector<std::optional<long long>>{0, 4, 2, 3, std::nullopt}));
  EXPECT_EQ(result.pathTo(3), (std::vector<int>{0, 1, 2, 3}));
  EXPECT_FALSE(result.hasNegativeCycle);
  EXPECT_FALSE(result.reachable(4));
}
// cpsc:test:end

// cpsc:test:start
TEST(BellmanFord, DetectsNegativeCycleAndPropagatesItsEffect) {
  using Edge = std::pair<int, int>;
  std::vector<std::vector<Edge>> graph{{{1, 1}}, {{2, -2}}, {{1, 1}, {3, 4}},
                                       {},       {{5, -1}}, {{4, 0}}};
  auto result = bellmanFord<int, int>(graph, std::initializer_list{0});

  EXPECT_TRUE(result.hasNegativeCycle);
  EXPECT_FALSE(result.isNegativeInfinity(0));
  EXPECT_TRUE(result.isNegativeInfinity(1));
  EXPECT_TRUE(result.isNegativeInfinity(2));
  EXPECT_TRUE(result.isNegativeInfinity(3));
  EXPECT_FALSE(result.isNegativeInfinity(4));
  EXPECT_TRUE(result.pathTo(3).empty());
}
// cpsc:test:end

// cpsc:test:start
TEST(BellmanFord, IgnoresUnreachableNegativeCycle) {
  using Edge = std::pair<int, int>;
  std::vector<std::vector<Edge>> graph{
      {{1, 2}}, {{2, 3}}, {}, {{4, -1}}, {{3, 0}}};
  auto result = bellmanFord<int, int>(graph, std::initializer_list{0});

  EXPECT_FALSE(result.hasNegativeCycle);
  EXPECT_FALSE(result.isNegativeInfinity(3));
  EXPECT_FALSE(result.reachable(3));
}
// cpsc:test:end

// cpsc:test:start
TEST(BellmanFord, SupportsMultipleStarts) {
  using Edge = std::pair<unsigned int, int>;
  std::vector<std::vector<Edge>> graph{
      {{1, 5}}, {{2, 5}}, {}, {{4, -2}}, {{3, 1}}};
  std::vector<unsigned int> starts{0, 3};
  auto result = bellmanFord<unsigned int, int>(graph, starts);

  EXPECT_TRUE(result.hasNegativeCycle);
  EXPECT_EQ(result.distance[0], 0);
  EXPECT_TRUE(result.isNegativeInfinity(3));
  EXPECT_TRUE(result.isNegativeInfinity(4));
}
// cpsc:test:end

// cpsc:test:start
TEST(BellmanFord, AvoidsIntegralOverflowAndUnderflow) {
  using Edge = std::pair<int, int>;
  const int maximum = std::numeric_limits<int>::max();
  const int minimum = std::numeric_limits<int>::lowest();
  std::vector<std::vector<Edge>> graph{
      {{1, maximum}, {3, minimum}}, {{2, 1}}, {}, {{4, -1}}, {}};
  auto result = bellmanFord<int, int>(graph, std::initializer_list{0});

  EXPECT_EQ(result.distance[1], maximum);
  EXPECT_EQ(result.distance[2], std::nullopt);
  EXPECT_EQ(result.distance[3], minimum);
  EXPECT_EQ(result.distance[4], std::nullopt);
}
// cpsc:test:end
