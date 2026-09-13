/* cpsc:meta:start
snippet: eratosthenes-sieve
cpsc:meta:end */

#include "test-support.hh"
#include "eratosthenes-sieve.hh"
#include "linear-sieve.hh"

// cpsc:test:start
TEST(EratosthenesSieve, MatchesLinearSieveQueries) {
  const EratosthenesSieve eratosthenes(1'000);
  const LinearSieve linear(1'000);

  EXPECT_EQ(eratosthenes.maxN(), linear.maxN());
  EXPECT_EQ(eratosthenes.primes(), linear.primes());
  for (int value = 1; value <= eratosthenes.maxN(); ++value) {
    EXPECT_EQ(eratosthenes.isPrime(value), linear.isPrime(value));
    EXPECT_EQ(eratosthenes.minFactor(value), linear.minFactor(value));
    EXPECT_EQ(eratosthenes.primeFactorize(value), linear.primeFactorize(value));
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(EratosthenesSieve, GeneratesWithDifferentMonoidOperations) {
  const EratosthenesSieve sieve(100);

  const auto phi = sieve.generate<int>(
      1, std::multiplies{}, [](int prime, int, int primePower) {
        return primePower / prime * (prime - 1);
      });
  const auto omega =
      sieve.generate<int>(0, std::plus{}, [](int, int, int) { return 1; });
  const auto factorText = sieve.generate<std::string>(
      std::string{}, std::plus{}, [](int prime, int exponent, int) {
        return std::to_string(prime) + "^" + std::to_string(exponent) + " ";
      });

  EXPECT_EQ(phi[60], 16);
  EXPECT_EQ(omega[60], 3);
  EXPECT_EQ(factorText[60], "2^2 3^1 5^1 ");
}
// cpsc:test:end

// cpsc:test:start
TEST(EratosthenesSieve, HandlesSmallestRangeAndIntegralTypes) {
  const EratosthenesSieve<long long> smallest(1);
  const EratosthenesSieve<unsigned int> unsignedSieve(100U);

  EXPECT_TRUE(smallest.primes().empty());
  EXPECT_FALSE(smallest.isPrime(1));
  EXPECT_EQ(smallest.minFactor(1), 1);
  EXPECT_TRUE(smallest.primeFactorize(1).empty());
  EXPECT_EQ(unsignedSieve.primes().back(), 97U);
  EXPECT_EQ(unsignedSieve.minFactor(91U), 7U);
}
// cpsc:test:end
