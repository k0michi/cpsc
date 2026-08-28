/* cpsc:meta:start
snippet: primality-test
cpsc:meta:end */

#include "test-support.hh"
#include "primality-test.hh"

// cpsc:test:start
TEST(PrimalityTest, RejectsValuesBelowTwo) {
  EXPECT_FALSE(isPrime(-17));
  EXPECT_FALSE(isPrime(0));
  EXPECT_FALSE(isPrime(1));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalityTest, HandlesSmallIntegers) {
  EXPECT_TRUE(isPrime(2));
  EXPECT_TRUE(isPrime(3));
  EXPECT_TRUE(isPrime(5));
  EXPECT_FALSE(isPrime(4));
  EXPECT_FALSE(isPrime(6));
  EXPECT_FALSE(isPrime(9));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalityTest, DistinguishesPrimesAndComposites) {
  EXPECT_TRUE(isPrime(97));
  EXPECT_TRUE(isPrime(104729));
  EXPECT_FALSE(isPrime(91));
  EXPECT_FALSE(isPrime(104730));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalityTest, AvoidsOverflowNearIntegerLimits) {
  EXPECT_TRUE(isPrime(2147483647));
  EXPECT_FALSE(isPrime(4293001441U));
  EXPECT_FALSE(isPrime(std::numeric_limits<unsigned int>::max()));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimalityTest, SupportsUnsignedAndConstantEvaluation) {
  static_assert(isPrime(2U));
  static_assert(isPrime(101LL));
  static_assert(!isPrime(100ULL));

  EXPECT_TRUE(isPrime(static_cast<unsigned short>(251)));
}
// cpsc:test:end
