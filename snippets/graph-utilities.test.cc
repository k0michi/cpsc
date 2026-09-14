/* cpsc:meta:start
snippet: graph-utilities
cpsc:meta:end */

#include "test-support.hh"
#include "graph-utilities.hh"

// cpsc:test:start
TEST(GraphUtilities, ReadsAdjacentEdges) {
  struct Edge {
    int to;
    long long cost;
    int weight;
  };
  const Edge member{.to = 3, .cost = 7, .weight = 0};
  const std::pair<int, long long> tuple{4, 9};

  EXPECT_EQ(getEdgeTarget(2), 2);
  EXPECT_EQ(getEdgeTarget(member), 3);
  EXPECT_EQ(getEdgeCost(member), 7);
  EXPECT_FALSE(getEdgeWeightBool(member));
  EXPECT_EQ(getEdgeTarget(tuple), 4);
  EXPECT_EQ(getEdgeCost(tuple), 9);
  EXPECT_TRUE(getEdgeWeightBool(tuple));
}
// cpsc:test:end

// cpsc:test:start
TEST(GraphUtilities, ReadsFullEdges) {
  struct Edge {
    unsigned int from;
    unsigned int to;
    long long cost;
  };
  const Edge member{.from = 1, .to = 2, .cost = -3};
  const std::tuple<int, int, int> tuple{3, 4, 5};

  EXPECT_EQ(getFullEdgeSource(member), 1U);
  EXPECT_EQ(getFullEdgeTarget(member), 2U);
  EXPECT_EQ(getFullEdgeCost(member), -3);
  EXPECT_EQ(getFullEdgeSource(tuple), 3);
  EXPECT_EQ(getFullEdgeTarget(tuple), 4);
  EXPECT_EQ(getFullEdgeCost(tuple), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(GraphUtilities, ValidatesVerticesAndRestoresPath) {
  const std::vector<std::optional<int>> previous{std::nullopt, 0, 1,
                                                 std::nullopt};

  EXPECT_TRUE(isValidVertex(0, 3U));
  EXPECT_TRUE(isValidVertex(2U, 3));
  EXPECT_FALSE(isValidVertex(-1, 3U));
  EXPECT_FALSE(isValidVertex(3U, 3));
  EXPECT_EQ(restorePath(2, previous), (std::vector<int>{0, 1, 2}));
}
// cpsc:test:end
