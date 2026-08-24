/* cpsc:meta:start
snippet: integer-logarithm
cpsc:meta:end */

#include "test-support.hpp"
#include "integer-logarithm.cpp"

// cpsc:test:start
TEST(IntegerLogarithm, HandlesBinaryBoundaries) {
  EXPECT_EQ(floorLog2(1), 0);
  EXPECT_EQ(floorLog2(2), 1);
  EXPECT_EQ(floorLog2(3), 1);
  EXPECT_EQ(floorLog2(1023), 9);
  EXPECT_EQ(floorLog2(1024), 10);

  EXPECT_EQ(ceilLog2(0), 0);
  EXPECT_EQ(ceilLog2(1), 0);
  EXPECT_EQ(ceilLog2(2), 1);
  EXPECT_EQ(ceilLog2(3), 2);
  EXPECT_EQ(ceilLog2(1024), 10);
  EXPECT_EQ(ceilLog2(1025), 11);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerLogarithm, HandlesArbitraryBases) {
  EXPECT_EQ(floorLog(3LL, 1LL), 0);
  EXPECT_EQ(floorLog(3LL, 8LL), 1);
  EXPECT_EQ(floorLog(3LL, 9LL), 2);
  EXPECT_EQ(floorLog(10LL, 999LL), 2);
  EXPECT_EQ(floorLog(10LL, 1000LL), 3);

  EXPECT_EQ(ceilLog(3LL, 1LL), 0);
  EXPECT_EQ(ceilLog(3LL, 8LL), 2);
  EXPECT_EQ(ceilLog(3LL, 9LL), 2);
  EXPECT_EQ(ceilLog(10LL, 1000LL), 3);
  EXPECT_EQ(ceilLog(10LL, 1001LL), 4);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerLogarithm, HandlesIntegerLimitsWithoutOverflow) {
  EXPECT_EQ(floorLog2(std::numeric_limits<unsigned long long>::max()), 63);
  EXPECT_EQ(ceilLog2(std::numeric_limits<unsigned long long>::max()), 64);
  EXPECT_EQ(floorLog(2ULL, std::numeric_limits<unsigned long long>::max()), 63);
  EXPECT_EQ(ceilLog(2ULL, std::numeric_limits<unsigned long long>::max()), 64);
  EXPECT_EQ(ceilLog(10LL, std::numeric_limits<long long>::max()), 19);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerLogarithm, SupportsConstantEvaluation) {
  static_assert(floorLog2(16) == 4);
  static_assert(ceilLog2(17) == 5);
  static_assert(floorLog(5, 124) == 2);
  static_assert(ceilLog(5, 126) == 4);
  SUCCEED();
}
// cpsc:test:end
