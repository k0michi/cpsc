/* cpsc:meta:start
snippet: prime-factorization
cpsc:meta:end */

#include "test-support.hh"
#include "prime-factorization.hh"

// cpsc:test:start
TEST(PrimeFactorization, FactorizesCompositeNumber) {
  EXPECT_EQ(primeFactorize(360LL),
            (std::vector<std::pair<long long, int>>{{2, 3}, {3, 2}, {5, 1}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimeFactorization, HandlesOneAndPrimeNumbers) {
  EXPECT_TRUE(primeFactorize(1).empty());
  EXPECT_EQ(primeFactorize(2), (std::vector<std::pair<int, int>>{{2, 1}}));
  EXPECT_EQ(primeFactorize(104729),
            (std::vector<std::pair<int, int>>{{104729, 1}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimeFactorization, CountsPrimePowers) {
  EXPECT_EQ(primeFactorize(1024), (std::vector<std::pair<int, int>>{{2, 10}}));
  EXPECT_EQ(primeFactorize(59049), (std::vector<std::pair<int, int>>{{3, 10}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimeFactorization, HandlesSquaresAndRepeatedDivision) {
  EXPECT_EQ(primeFactorize(49), (std::vector<std::pair<int, int>>{{7, 2}}));
  EXPECT_EQ(primeFactorize(12),
            (std::vector<std::pair<int, int>>{{2, 2}, {3, 1}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimeFactorization, SupportsUnsignedAndConstantEvaluation) {
  static_assert([] {
    auto factors = primeFactorize(255U);
    return factors == std::vector<std::pair<unsigned int, int>>{
                          {3U, 1}, {5U, 1}, {17U, 1}};
  }());

  EXPECT_EQ(primeFactorize(std::numeric_limits<unsigned int>::max()),
            (std::vector<std::pair<unsigned int, int>>{
                {3U, 1}, {5U, 1}, {17U, 1}, {257U, 1}, {65537U, 1}}));
}
// cpsc:test:end
