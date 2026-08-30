/* cpsc:meta:start
snippet: bfs
cpsc:meta:end */

#include "test-support.hh"
#include "bfs.hh"

// cpsc:test:start
TEST(Bfs, FindsShortestEdgeCounts) {
  std::vector<std::vector<int>> graph{{1, 2}, {3}, {3, 4}, {5}, {}, {}};
  auto result = bfs<int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<int>>{0, 1, 1, 2, 2, 3}));
  EXPECT_EQ(result.pathTo(5), (std::vector<int>{0, 1, 3, 5}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, MarksUnreachableVertices) {
  std::vector<std::vector<int>> graph{{1}, {}, {3}, {}};
  auto result = bfs<int>(graph, std::initializer_list{2});
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
  EXPECT_EQ(bfs<int>(memberGraph, std::initializer_list{0}).distance,
            (std::vector<std::optional<int>>{0, 1, 2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, RunsZeroOneBfsWithTupleEdges) {
  using Edge = std::pair<int, bool>;
  std::vector<std::vector<Edge>> graph{
      {{1, true}, {2, false}}, {{3, true}}, {{1, false}, {3, true}}, {}};
  auto result = bfs<int>(graph, std::initializer_list{0});
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
  auto result = bfs<int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 0, 1}));
  EXPECT_EQ(result.previous[1], 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsUnsignedVertexAndDistanceType) {
  std::vector<std::vector<unsigned int>> graph{{1}, {2}, {}};
  auto result = bfs<unsigned int>(graph, std::initializer_list{0U});
  EXPECT_EQ(result.distance,
            (std::vector<std::optional<unsigned int>>{0U, 1U, 2U}));
  EXPECT_EQ(result.pathTo(2U), (std::vector<unsigned int>{0U, 1U, 2U}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsMultipleStartsFromVector) {
  std::vector<std::vector<int>> graph{{1}, {0, 2}, {1, 3}, {2, 4}, {3}};
  std::vector<int> starts{0, 4};
  auto result = bfs<int>(graph, starts);

  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 1, 2, 1, 0}));
  EXPECT_EQ(result.pathTo(2), (std::vector<int>{0, 1, 2}));
  EXPECT_EQ(result.pathTo(3), (std::vector<int>{4, 3}));
  EXPECT_EQ(result.previous[0], std::nullopt);
  EXPECT_EQ(result.previous[4], std::nullopt);
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsMultipleStartsFromInitializerList) {
  std::vector<std::vector<int>> graph{{1}, {0, 2}, {1, 3}, {2}};
  auto result = bfs<int>(graph, std::initializer_list{0, 3});

  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 1, 1, 0}));
  EXPECT_EQ(result.pathTo(2), (std::vector<int>{3, 2}));
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
  auto result = bfs<int>(graph, std::initializer_list{0});
  EXPECT_EQ(result.distance, (std::vector<std::optional<int>>{0, 0, 0, 1}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Bfs, SupportsGridReturningNeighborVectors) {
  struct Point {
    int row;
    int column;

    [[nodiscard]] constexpr Point operator+(const Point &other) const {
      return {.row = row + other.row, .column = column + other.column};
    }
  };

  struct Grid2DIndexer {
    int height;
    int width;

    [[nodiscard]] int index(Point point) const {
      return point.row * width + point.column;
    }

    [[nodiscard]] Point coordinates(int vertex) const {
      return {vertex / width, vertex % width};
    }

    [[nodiscard]] bool contains(Point point) const {
      return 0 <= point.row && point.row < height && 0 <= point.column &&
             point.column < width;
    }

    [[nodiscard]] std::size_t size() const {
      return static_cast<std::size_t>(height * width);
    }
  };

  struct Grid2D {
    Grid2DIndexer indexer;
    std::vector<std::string> cells;

    [[nodiscard]] std::size_t size() const {
      return indexer.size();
    }

    [[nodiscard]] std::vector<int> operator[](int vertex) const {
      Point current = indexer.coordinates(vertex);
      constexpr Point directions[] = {
          {.row = -1, .column = 0},
          {.row = 0, .column = 1},
          {.row = 1, .column = 0},
          {.row = 0, .column = -1},
      };
      std::vector<int> neighbors;
      for (Point direction : directions) {
        Point next = current + direction;
        if (indexer.contains(next) && cells[next.row][next.column] != '#') {
          neighbors.push_back(indexer.index(next));
        }
      }
      return neighbors;
    }
  };

  Grid2D grid{
      .indexer = {.height = 3, .width = 3},
      .cells = {"...", ".#.", "..."},
  };
  int start = grid.indexer.index({.row = 0, .column = 0});
  int goal = grid.indexer.index({.row = 2, .column = 2});
  auto result = bfs<int>(grid, std::initializer_list{start});

  EXPECT_EQ(result.distance[goal], 4);
  EXPECT_EQ(result.pathTo(goal),
            (std::vector<int>{grid.indexer.index({.row = 0, .column = 0}),
                              grid.indexer.index({.row = 0, .column = 1}),
                              grid.indexer.index({.row = 0, .column = 2}),
                              grid.indexer.index({.row = 1, .column = 2}),
                              grid.indexer.index({.row = 2, .column = 2})}));
  EXPECT_FALSE(result.reachable(grid.indexer.index({.row = 1, .column = 1})));
}
// cpsc:test:end
