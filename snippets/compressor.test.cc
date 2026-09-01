/* cpsc:meta:start
snippet: compressor
cpsc:meta:end */

#include "test-support.hh"
#include "compressor.hh"

// cpsc:test:start
TEST(Compressor, SortsDeduplicatesAndMapsCoordinates) {
  Compressor compressor{30, 10, 20, 10, 30};

  EXPECT_EQ(compressor.size(), 3);
  EXPECT_EQ(compressor.values(), (std::vector{10, 20, 30}));
  EXPECT_EQ(compressor.get(10), 0);
  EXPECT_EQ(compressor(20), 1);
  EXPECT_EQ(compressor.get(25), 2);
  EXPECT_EQ(compressor[2], 30);
  EXPECT_EQ(compressor.stepWidth(1), 10);
}
// cpsc:test:end

// cpsc:test:start
TEST(Compressor, BuildsFromInputRangesAndHandlesEmptyInput) {
  std::deque<long long> values{8, -2, 8, 3};
  Compressor compressor(values);
  Compressor<int> empty(std::vector<int>{});

  EXPECT_EQ(compressor.values(), (std::vector{-2LL, 3LL, 8LL}));
  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(empty.size(), 0);
  EXPECT_EQ(empty.get(4), 0);
}
// cpsc:test:end
