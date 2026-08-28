/* cpsc:meta:start
snippet: divisors
cpsc:meta:end */

#include "test-support.hh"
#include "divisors.hh"

// cpsc:test:start
TEST(Divisors, EnumeratesDivisorsInAscendingOrder) {
  EXPECT_EQ(divisors(12), (std::vector<int>{1, 2, 3, 4, 6, 12}));
  EXPECT_EQ(
      divisors(360),
      (std::vector<int>{1,  2,  3,  4,  5,  6,  8,  9,  10, 12,  15,  18,
                        20, 24, 30, 36, 40, 45, 60, 72, 90, 120, 180, 360}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Divisors, HandlesOneAndPrimeNumbers) {
  EXPECT_EQ(divisors(1), (std::vector<int>{1}));
  EXPECT_EQ(divisors(2), (std::vector<int>{1, 2}));
  EXPECT_EQ(divisors(101), (std::vector<int>{1, 101}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Divisors, DoesNotDuplicateSquareRoot) {
  EXPECT_EQ(divisors(36), (std::vector<int>{1, 2, 3, 4, 6, 9, 12, 18, 36}));
  EXPECT_EQ(divisors(49), (std::vector<int>{1, 7, 49}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Divisors, SupportsUnsignedAndConstantEvaluation) {
  static_assert([] {
    return divisors(16U) == std::vector<unsigned int>{1, 2, 4, 8, 16};
  }());

  EXPECT_EQ(divisors(std::numeric_limits<unsigned int>::max()),
            (std::vector<unsigned int>{
                1U,          3U,         5U,         15U,        17U,
                51U,         85U,        255U,       257U,       771U,
                1285U,       3855U,      4369U,      13107U,     21845U,
                65535U,      65537U,     196611U,    327685U,    983055U,
                1114129U,    3342387U,   5570645U,   16711935U,  16843009U,
                50529027U,   84215045U,  252645135U, 286331153U, 858993459U,
                1431655765U, 4294967295U}));
}
// cpsc:test:end
