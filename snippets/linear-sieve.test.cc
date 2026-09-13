/* cpsc:meta:start
snippet: linear-sieve
cpsc:meta:end */

#include "test-support.hh"
#include "linear-sieve.hh"

// cpsc:test:start
TEST(LinearSieve, EnumeratesPrimesAndMinimumFactors) {
  const LinearSieve sieve(30);

  EXPECT_EQ(sieve.maxN(), 30);
  EXPECT_EQ(sieve.primes(),
            (std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}));
  EXPECT_FALSE(sieve.isPrime(0));
  EXPECT_FALSE(sieve.isPrime(1));
  EXPECT_TRUE(sieve.isPrime(29));
  EXPECT_FALSE(sieve.isPrime(30));
  EXPECT_EQ(sieve.minFactor(1), 1);
  EXPECT_EQ(sieve.minFactor(25), 5);
  EXPECT_EQ(sieve.minFactor(30), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(LinearSieve, FactorizesWithinPrecomputedRange) {
  const LinearSieve sieve(1'000);

  EXPECT_TRUE(sieve.primeFactorize(1).empty());
  EXPECT_EQ(sieve.primeFactorize(2),
            (std::vector<std::pair<int, int>>{{2, 1}}));
  EXPECT_EQ(sieve.primeFactorize(360),
            (std::vector<std::pair<int, int>>{{2, 3}, {3, 2}, {5, 1}}));
  EXPECT_EQ(sieve.primeFactorize(997),
            (std::vector<std::pair<int, int>>{{997, 1}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(LinearSieve, GeneratesMultiplicativeFunctionTables) {
  const LinearSieve sieve(100);

  const auto divisorCount =
      sieve.generate<int>(1, std::multiplies{},
                          [](int, int exponent, int) { return exponent + 1; });
  const auto eulerPhi = sieve.generate<int>(
      1, std::multiplies{}, [](int prime, int, int primePower) {
        return primePower / prime * (prime - 1);
      });
  const auto mobius =
      sieve.generate<int>(1, std::multiplies{}, [](int, int exponent, int) {
        return exponent == 1 ? -1 : 0;
      });

  EXPECT_EQ(divisorCount[1], 1);
  EXPECT_EQ(divisorCount[60], 12);
  EXPECT_EQ(divisorCount[72], 12);
  EXPECT_EQ(eulerPhi[1], 1);
  EXPECT_EQ(eulerPhi[60], 16);
  EXPECT_EQ(eulerPhi[97], 96);
  EXPECT_EQ(mobius[1], 1);
  EXPECT_EQ(mobius[30], -1);
  EXPECT_EQ(mobius[60], 0);
}
// cpsc:test:end

// cpsc:test:start
TEST(LinearSieve, HandlesSmallestPrecomputationRange) {
  const LinearSieve sieve(1);
  const auto values = sieve.generate<long long>(
      1, std::multiplies{}, [](int, int, int) { return 0LL; });

  EXPECT_TRUE(sieve.primes().empty());
  EXPECT_FALSE(sieve.isPrime(1));
  EXPECT_EQ(sieve.minFactor(1), 1);
  EXPECT_TRUE(sieve.primeFactorize(1).empty());
  EXPECT_EQ(values, (std::vector<long long>{0, 1}));
}
// cpsc:test:end

// cpsc:test:start
TEST(LinearSieve, SupportsOtherIntegralTypes) {
  const LinearSieve<long long> signedSieve(1'000);
  const LinearSieve<unsigned int> unsignedSieve(100U);

  EXPECT_EQ(
      signedSieve.primeFactorize(840LL),
      (std::vector<std::pair<long long, int>>{{2, 3}, {3, 1}, {5, 1}, {7, 1}}));
  EXPECT_EQ(unsignedSieve.primes().back(), 97U);
  EXPECT_EQ(unsignedSieve.minFactor(91U), 7U);

  const auto radical = unsignedSieve.generate<unsigned int>(
      1U, std::multiplies{},
      [](unsigned int prime, int, unsigned int) { return prime; });
  EXPECT_EQ(radical[60], 30U);
}
// cpsc:test:end

// cpsc:test:start
TEST(LinearSieve, GeneratesWithDifferentMonoidOperations) {
  const LinearSieve sieve(100);

  const auto omega =
      sieve.generate<int>(0, std::plus{}, [](int, int, int) { return 1; });
  const auto maximumExponent = sieve.generate<int>(
      0, [](int left, int right) { return std::max(left, right); },
      [](int, int exponent, int) { return exponent; });
  const auto factorText = sieve.generate<std::string>(
      std::string{}, std::plus{}, [](int prime, int exponent, int) {
        return std::to_string(prime) + "^" + std::to_string(exponent) + " ";
      });

  EXPECT_EQ(omega[60], 3);
  EXPECT_EQ(omega[72], 2);
  EXPECT_EQ(maximumExponent[60], 2);
  EXPECT_EQ(maximumExponent[72], 3);
  EXPECT_EQ(factorText[60], "2^2 3^1 5^1 ");
}
// cpsc:test:end
