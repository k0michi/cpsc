/* cpsc:meta:start
snippet: modint
cpsc:meta:end */

#include "test-support.hpp"
#include "modint.cpp"

using Mint = ModInt<1'000'000'007>;

// cpsc:test:start
TEST(ExtendedGCD, ComputesGcdAndBezoutCoefficients) {
  auto [gcd, x, y] = extendedGCD(240, 46);

  EXPECT_EQ(gcd, 2);
  EXPECT_EQ(240 * x + 46 * y, gcd);
  EXPECT_EQ(extendedGCD(17, 0), (std::tuple{17LL, 1LL, 0LL}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, NormalizesSignedAndUnsignedValues) {
  EXPECT_EQ(Mint(42).val(), 42);
  EXPECT_EQ(Mint(-1).toLLong(), 1'000'000'006);
  EXPECT_EQ(Mint(1'000'000'008LL).toLLong(), 1);
  EXPECT_EQ(Mint(2'000'000'015ULL).toULLong(), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, SupportsArithmeticAndIncrementOperators) {
  Mint a = 1'000'000'006;
  Mint b = 2;

  EXPECT_EQ((a + b).toLLong(), 1);
  EXPECT_EQ((Mint(1) - b).toLLong(), 1'000'000'006);
  EXPECT_EQ((Mint(3) * Mint(4)).toLLong(), 12);
  EXPECT_EQ((Mint(10) / Mint(2)).toLLong(), 5);

  EXPECT_EQ((++a).toLLong(), 0);
  EXPECT_EQ((a++).toLLong(), 0);
  EXPECT_EQ(a.toLLong(), 1);
  EXPECT_EQ((--a).toLLong(), 0);
  EXPECT_EQ((a--).toLLong(), 0);
  EXPECT_EQ(a.toLLong(), 1'000'000'006);
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, MultipliesWithoutUnsignedLongLongOverflow) {
  using LargeMint = ModInt<9'223'372'036'854'775'123ULL>;
  unsigned long long value = 9'000'000'000'000'000'000ULL;
  unsigned long long expected = static_cast<unsigned long long>(
      static_cast<__uint128_t>(value) * value % LargeMint::Modulus);

  EXPECT_EQ((LargeMint(value) * LargeMint(value)).val(), expected);
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, ComputesPowersAndInverses) {
  EXPECT_EQ(Mint(2).pow(10).toLLong(), 1024);
  EXPECT_EQ(Mint::pow(Mint(2), 10).toLLong(), 1024);
  EXPECT_EQ(Mint::pow(Mint(123), 0).toLLong(), 1);
  EXPECT_EQ((Mint(3) * Mint(3).inv()).toLLong(), 1);
  EXPECT_EQ((Mint(3) * Mint::invert(Mint(3))).toLLong(), 1);
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, SupportsStreams) {
  std::stringstream stream;
  Mint value = 42;

  stream << value;
  EXPECT_EQ(stream.str(), "42");

  stream.str("-1000000008");
  stream.clear();
  stream >> value;
  EXPECT_EQ(value.toLLong(), 1'000'000'006);
}
// cpsc:test:end
