/* cpsc:meta:start
snippet: integer-division
cpsc:meta:end */

#include "test-support.hpp"
#include "integer-division.cpp"

// cpsc:test:start
TEST(IntegerDivision, RoundsQuotientsInTheRequestedDirection) {
  EXPECT_EQ(floorDiv(7, 3), 2);
  EXPECT_EQ(floorDiv(-7, 3), -3);
  EXPECT_EQ(floorDiv(7, -3), -3);
  EXPECT_EQ(floorDiv(-7, -3), 2);

  EXPECT_EQ(ceilDiv(7, 3), 3);
  EXPECT_EQ(ceilDiv(-7, 3), -2);
  EXPECT_EQ(ceilDiv(7, -3), -2);
  EXPECT_EQ(ceilDiv(-7, -3), 3);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerDivision, KeepsExactDivisionUnchanged) {
  EXPECT_EQ(floorDiv(12LL, 4LL), 3LL);
  EXPECT_EQ(ceilDiv(12LL, 4LL), 3LL);
  EXPECT_EQ(floorMod(12LL, 4LL), 0LL);
  EXPECT_EQ(ceilMod(12LL, 4LL), 0LL);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerDivision, ReturnsRemaindersConsistentWithQuotients) {
  for (long long a : {-7LL, -1LL, 0LL, 1LL, 7LL}) {
    for (long long b : {-3LL, -2LL, 2LL, 3LL}) {
      EXPECT_EQ(a, floorDiv(a, b) * b + floorMod(a, b));
      EXPECT_EQ(a, ceilDiv(a, b) * b + ceilMod(a, b));
    }
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerDivision, SupportsConstantEvaluation) {
  static_assert(floorDiv(-7, 3) == -3);
  static_assert(floorMod(-7, 3) == 2);
  static_assert(ceilDiv(7, 3) == 3);
  static_assert(ceilMod(7, 3) == -2);
  SUCCEED();
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerDivision, HandlesZeroAndIntegerLimits) {
  EXPECT_EQ(floorDiv(0LL, -7LL), 0);
  EXPECT_EQ(floorMod(0LL, -7LL), 0);
  EXPECT_EQ(ceilDiv(0LL, 7LL), 0);
  EXPECT_EQ(ceilMod(0LL, 7LL), 0);

  long long min = std::numeric_limits<long long>::min();
  long long max = std::numeric_limits<long long>::max();
  EXPECT_EQ(floorDiv(min, 2LL), min / 2);
  EXPECT_EQ(ceilDiv(max, 2LL), max / 2 + 1);
  EXPECT_EQ(floorMod(min, 2LL), 0);
  EXPECT_EQ(ceilMod(max, 2LL), -1);
}
// cpsc:test:end
