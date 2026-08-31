/* cpsc:meta:start
snippet: int128
cpsc:meta:end */

#include "test-support.hh"
#include "int128.hh"

#include <sstream>

// cpsc:test:start
TEST(UInt128, ConstructsConvertsAndCompares) {
  UInt128 value{1, 2};

  EXPECT_EQ(value.high(), 1);
  EXPECT_EQ(value.low(), 2);
  EXPECT_EQ(static_cast<unsigned long long>(value), 2);
  EXPECT_GT(value, UInt128(std::numeric_limits<unsigned long long>::max()));
  EXPECT_EQ(UInt128(-1), std::numeric_limits<UInt128>::max());
  EXPECT_FALSE(static_cast<bool>(UInt128{}));
  EXPECT_TRUE(static_cast<bool>(value));
}
// cpsc:test:end

// cpsc:test:start
TEST(UInt128, SupportsArithmeticAndBitOperators) {
  UInt128 power = UInt128{1} << 100;

  EXPECT_EQ((power + 7).high(), std::uint64_t{1} << 36);
  EXPECT_EQ((power + 7).low(), 7);
  EXPECT_EQ((power - 1).low(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ((UInt128{1, 0} * 3), UInt128(3, 0));
  EXPECT_EQ((power + 12345) / 97 * 97 + (power + 12345) % 97, power + 12345);
  EXPECT_EQ((UInt128{0xf0} & UInt128{0x3c}), UInt128{0x30});
  EXPECT_EQ((UInt128{0xf0} | UInt128{0x0f}), UInt128{0xff});
  EXPECT_EQ((UInt128{0xf0} ^ UInt128{0xff}), UInt128{0x0f});
  EXPECT_EQ((UInt128{1, 2} >> 64), UInt128{1});
  EXPECT_EQ((UInt128{1} << 128), UInt128{});
}
// cpsc:test:end

// cpsc:test:start
TEST(Int128, SupportsSignedArithmeticComparisonAndShift) {
  Int128 large = Int128(UInt128{1} << 100);

  EXPECT_LT(Int128(-1), Int128(0));
  EXPECT_EQ((-large + 5) / 3, -(large - 5) / 3);
  EXPECT_EQ(Int128(-100) / 7, Int128(-14));
  EXPECT_EQ(Int128(-100) % 7, Int128(-2));
  EXPECT_EQ(Int128(100) / -7, Int128(-14));
  EXPECT_EQ(Int128(-8) >> 2, Int128(-2));
  EXPECT_EQ(Int128(-1) >> 128, Int128(-1));
  EXPECT_EQ(std::numeric_limits<Int128>::max() + 1,
            std::numeric_limits<Int128>::min());
}
// cpsc:test:end

// cpsc:test:start
TEST(Int128, ConvertsToAndFromDecimalText) {
  const std::string unsignedMaximum = "340282366920938463463374607431768211455";
  const std::string signedMinimum = "-170141183460469231731687303715884105728";
  std::stringstream stream(unsignedMaximum + " " + signedMinimum);
  UInt128 unsignedValue;
  Int128 signedValue;

  stream >> unsignedValue >> signedValue;

  EXPECT_TRUE(stream);
  EXPECT_EQ(unsignedValue, std::numeric_limits<UInt128>::max());
  EXPECT_EQ(signedValue, std::numeric_limits<Int128>::min());
  EXPECT_EQ(unsignedValue.toString(), unsignedMaximum);
  EXPECT_EQ(signedValue.toString(), signedMinimum);
  std::stringstream output;
  output << Int128(-42);
  EXPECT_EQ(output.str(), "-42");
}
// cpsc:test:end

#if defined(__SIZEOF_INT128__)
// cpsc:test:start
TEST(Int128, MatchesNativeInt128Arithmetic) {
  UInt128 left{0x123456789abcdef0ULL, 0xfedcba9876543210ULL};
  UInt128 right{0, 0x1020304050607080ULL};
  auto nativeLeft = static_cast<unsigned __int128>(left);
  auto nativeRight = static_cast<unsigned __int128>(right);

  EXPECT_EQ(static_cast<unsigned __int128>(left + right),
            nativeLeft + nativeRight);
  EXPECT_EQ(static_cast<unsigned __int128>(left * right),
            nativeLeft * nativeRight);
  EXPECT_EQ(static_cast<unsigned __int128>(left / right),
            nativeLeft / nativeRight);
  EXPECT_EQ(static_cast<unsigned __int128>(left % right),
            nativeLeft % nativeRight);
}
// cpsc:test:end
#endif
