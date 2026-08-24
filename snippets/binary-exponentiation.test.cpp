/* cpsc:meta:start
snippet: binary-exponentiation
cpsc:meta:end */

#include "test-support.hpp"
#include "binary-exponentiation.cpp"

// cpsc:test:start
TEST(BinaryPow, ComputesNonnegativePowers) {
  EXPECT_EQ(binaryPow(2LL, 10), 1024);
  EXPECT_EQ(binaryPow(5LL, 1), 5);
  EXPECT_EQ(binaryPow(7LL, 0), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(BinaryPow, SupportsUnsignedExponents) {
  EXPECT_EQ(binaryPow(3LL, 5ULL), 243);
}
// cpsc:test:end

// cpsc:test:start
TEST(BinaryPow, HandlesIdentityAndAbsorbingElements) {
  EXPECT_EQ(binaryPow(0LL, 0), 1);
  EXPECT_EQ(binaryPow(0LL, 100), 0);
  EXPECT_EQ(binaryPow(-1LL, 1'000'001), -1);
  EXPECT_EQ(binaryPow(-1LL, 1'000'000), 1);
}
// cpsc:test:end
