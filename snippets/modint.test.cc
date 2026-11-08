/* cpsc:meta:start
snippet: modint
cpsc:meta:end */

#include "test-support.hh"
#include "modint.hh"

using Mint = ModInt<1'000'000'007>;

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
TEST(ModInt, MultipliesAcrossUint32ModulusBoundary) {
  using Uint32Mint = ModInt<std::numeric_limits<std::uint32_t>::max()>;
  using AboveUint32Mint =
      ModInt<static_cast<unsigned long long>(
                 std::numeric_limits<std::uint32_t>::max()) +
             2>;

  constexpr auto uint32Product =
      Uint32Mint(Uint32Mint::Modulus - 2) * Uint32Mint(Uint32Mint::Modulus - 3);
  constexpr auto aboveUint32Product =
      AboveUint32Mint(AboveUint32Mint::Modulus - 2) *
      AboveUint32Mint(AboveUint32Mint::Modulus - 3);

  static_assert(uint32Product.val() == 6);
  static_assert(aboveUint32Product.val() == 6);
}
// cpsc:test:end

// cpsc:test:start
TEST(ModInt, HandlesArithmeticNearLongLongMaximumModulus) {
  using MaximumMint = ModInt<static_cast<unsigned long long>(
      std::numeric_limits<long long>::max())>;

  constexpr auto modulus = MaximumMint::Modulus;
  static_assert((MaximumMint(modulus - 1) + MaximumMint(modulus - 1)).val() ==
                modulus - 2);
  static_assert((MaximumMint(1) - MaximumMint(2)).val() == modulus - 1);
  static_assert((MaximumMint(modulus - 2) * MaximumMint(modulus - 3)).val() ==
                6);

  const MaximumMint inverse = MaximumMint(2).inv();
  EXPECT_EQ((MaximumMint(2) * inverse).val(), 1);
  EXPECT_EQ(inverse.val(), (modulus + 1) / 2);
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

// cpsc:test:start
TEST(ModInt, HandlesZeroAndModulusOne) {
  using TrivialMint = ModInt<1>;

  EXPECT_EQ(Mint{}.val(), 0);
  EXPECT_EQ((-Mint{}).val(), 0);
  EXPECT_EQ(Mint(0).pow(0).val(), 1);
  EXPECT_EQ(TrivialMint(123).val(), 0);
  EXPECT_EQ((TrivialMint(0) + TrivialMint(0)).val(), 0);
  EXPECT_EQ(TrivialMint(42).pow(10).val(), 0);
}
// cpsc:test:end
