/* cpsc:meta:start
snippet: integer-overflow
cpsc:meta:end */

#include "test-support.hh"
#include "integer-overflow.hh"

#include <cstdint>

// cpsc:test:start
TEST(IntegerOverflow, ComputesAbsoluteDifferenceWithoutOverflow) {
  constexpr int minimum = std::numeric_limits<int>::min();
  constexpr int maximum = std::numeric_limits<int>::max();

  static_assert(absDiff(20, 62) == 42U);
  static_assert(absDiff(62, 20) == 42U);
  static_assert(absDiff(minimum, 0) ==
                static_cast<unsigned int>(maximum) + 1U);
  static_assert(absDiff(minimum, maximum) ==
                std::numeric_limits<unsigned int>::max());
  static_assert(absDiff(0U, std::numeric_limits<unsigned int>::max()) ==
                std::numeric_limits<unsigned int>::max());
  SUCCEED();
}
// cpsc:test:end

// cpsc:test:start
TEST(CheckedIntegerArithmetic, DetectsOverflowWithoutEvaluatingIt) {
  constexpr int minimum = std::numeric_limits<int>::min();
  constexpr int maximum = std::numeric_limits<int>::max();

  EXPECT_EQ(checkedAdd(20, 22), 42);
  EXPECT_FALSE(checkedAdd(maximum, 1));
  EXPECT_FALSE(checkedAdd(minimum, -1));
  EXPECT_FALSE(checkedSub(minimum, 1));
  EXPECT_FALSE(checkedSub(maximum, -1));
  EXPECT_EQ(checkedMul(-6, 7), -42);
  EXPECT_FALSE(checkedMul(maximum, 2));
  EXPECT_FALSE(checkedMul(minimum, -1));
  EXPECT_FALSE(checkedDiv(1, 0));
  EXPECT_FALSE(checkedDiv(minimum, -1));
  EXPECT_EQ(checkedDiv(-7, 3), -2);
}
// cpsc:test:end

// cpsc:test:start
TEST(CheckedIntegerArithmetic, SupportsNegAbsPowAndShifts) {
  constexpr int minimum = std::numeric_limits<int>::min();

  EXPECT_EQ(checkedNeg(-42), 42);
  EXPECT_FALSE(checkedNeg(minimum));
  EXPECT_EQ(checkedAbs(-42), 42);
  EXPECT_FALSE(checkedAbs(minimum));
  EXPECT_EQ(checkedPow(3, 4), 81);
  EXPECT_FALSE(checkedPow(std::numeric_limits<int>::max(), 2));
  EXPECT_EQ(checkedShl(std::uint8_t{1}, 7), std::uint8_t{128});
  EXPECT_FALSE(checkedShl(std::uint8_t{1}, 8));
  EXPECT_EQ(checkedShr(std::int8_t{-8}, 2), std::int8_t{-2});
  EXPECT_FALSE(checkedShr(std::int8_t{-8}, 8));
}
// cpsc:test:end

// cpsc:test:start
TEST(OverflowingIntegerArithmetic, ReturnsWrappedValueAndFlag) {
  constexpr auto maximum = std::numeric_limits<std::int8_t>::max();
  constexpr auto minimum = std::numeric_limits<std::int8_t>::min();

  EXPECT_EQ(overflowingAdd(maximum, std::int8_t{1}),
            (std::pair{minimum, true}));
  EXPECT_EQ(overflowingSub(minimum, std::int8_t{1}),
            (std::pair{maximum, true}));
  EXPECT_EQ(overflowingMul(std::int8_t{100}, std::int8_t{2}),
            (std::pair{std::int8_t{-56}, true}));
  EXPECT_EQ(overflowingDiv(minimum, std::int8_t{-1}),
            (std::pair{minimum, true}));
  EXPECT_EQ(overflowingNeg(minimum), (std::pair{minimum, true}));
  EXPECT_EQ(overflowingPow(std::int8_t{5}, 4),
            (std::pair{std::int8_t{113}, true}));
  EXPECT_EQ(overflowingShl(std::uint8_t{1}, 9),
            (std::pair{std::uint8_t{2}, true}));
}
// cpsc:test:end

// cpsc:test:start
TEST(SaturatingIntegerArithmetic, ClampsAtIntegerBounds) {
  constexpr auto minimum = std::numeric_limits<int>::min();
  constexpr auto maximum = std::numeric_limits<int>::max();

  EXPECT_EQ(saturatingAdd(maximum, 1), maximum);
  EXPECT_EQ(saturatingAdd(minimum, -1), minimum);
  EXPECT_EQ(saturatingSub(minimum, 1), minimum);
  EXPECT_EQ(saturatingMul(minimum, -1), maximum);
  EXPECT_EQ(saturatingMul(minimum, 2), minimum);
  EXPECT_EQ(saturatingDiv(minimum, -1), maximum);
  EXPECT_EQ(saturatingNeg(minimum), maximum);
  EXPECT_EQ(saturatingAbs(minimum), maximum);
  EXPECT_EQ(saturatingPow(-100'000, 3), minimum);
  EXPECT_EQ(saturatingPow(-100'000, 4), maximum);
}
// cpsc:test:end

// cpsc:test:start
TEST(WrappingIntegerArithmetic, WrapsModuloTypeWidth) {
  constexpr auto maximum = std::numeric_limits<std::int8_t>::max();
  constexpr auto minimum = std::numeric_limits<std::int8_t>::min();

  EXPECT_EQ(wrappingAdd(maximum, std::int8_t{1}), minimum);
  EXPECT_EQ(wrappingSub(minimum, std::int8_t{1}), maximum);
  EXPECT_EQ(wrappingMul(std::int8_t{100}, std::int8_t{2}), -56);
  EXPECT_EQ(wrappingDiv(minimum, std::int8_t{-1}), minimum);
  EXPECT_EQ(wrappingNeg(minimum), minimum);
  EXPECT_EQ(wrappingAbs(minimum), minimum);
  EXPECT_EQ(wrappingPow(std::uint8_t{5}, 4), 113);
  EXPECT_EQ(wrappingShl(std::uint8_t{1}, 9), 2);
  EXPECT_EQ(wrappingShr(std::uint8_t{128}, 9), 64);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerCast, ProvidesCheckedOverflowingSaturatingAndWrappingPolicies) {
  EXPECT_EQ(checkedCast<std::int8_t>(127), std::int8_t{127});
  EXPECT_FALSE(checkedCast<std::int8_t>(128));
  EXPECT_FALSE(checkedCast<unsigned int>(-1));

  EXPECT_EQ(overflowingCast<std::int8_t>(255),
            (std::pair{std::int8_t{-1}, true}));
  EXPECT_EQ(overflowingCast<std::uint16_t>(std::int8_t{-1}),
            (std::pair{std::uint16_t{65535}, true}));

  EXPECT_EQ(saturatingCast<std::int8_t>(1000),
            std::numeric_limits<std::int8_t>::max());
  EXPECT_EQ(saturatingCast<std::int8_t>(-1000),
            std::numeric_limits<std::int8_t>::min());
  EXPECT_EQ(saturatingCast<unsigned int>(-1), 0U);
  EXPECT_EQ(wrappingCast<std::uint8_t>(-1), std::uint8_t{255});
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, SupportsConstantEvaluation) {
  static_assert(checkedAdd(20, 22) == 42);
  static_assert(!checkedMul(std::numeric_limits<int>::max(), 2));
  static_assert(wrappingAdd(std::uint8_t{255}, std::uint8_t{1}) == 0);
  static_assert(saturatingCast<std::int8_t>(1000) == 127);
  static_assert(wrappingCast<std::int8_t>(255) == -1);
  SUCCEED();
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, ExhaustivelyChecksEightBitArithmetic) {
  for (int left = std::numeric_limits<std::int8_t>::min();
       left <= std::numeric_limits<std::int8_t>::max(); ++left) {
    for (int right = std::numeric_limits<std::int8_t>::min();
         right <= std::numeric_limits<std::int8_t>::max(); ++right) {
      auto check = [](auto actual, int exact) {
        bool overflow = !std::in_range<std::int8_t>(exact);
        EXPECT_EQ(actual,
                  (std::pair{static_cast<std::int8_t>(exact), overflow}));
      };
      auto lhs = static_cast<std::int8_t>(left);
      auto rhs = static_cast<std::int8_t>(right);
      check(overflowingAdd(lhs, rhs), left + right);
      check(overflowingSub(lhs, rhs), left - right);
      check(overflowingMul(lhs, rhs), left * right);
    }
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, ExhaustivelyChecksMixedSignEightBitArithmetic) {
  auto checkSigned = [](auto actual, int exact) {
    bool overflow = !std::in_range<std::int8_t>(exact);
    EXPECT_EQ(actual, (std::pair{static_cast<std::int8_t>(exact), overflow}));
  };
  auto checkUnsigned = [](auto actual, int exact) {
    bool overflow = !std::in_range<std::uint8_t>(exact);
    EXPECT_EQ(actual, (std::pair{static_cast<std::uint8_t>(exact), overflow}));
  };
  for (int signedValue = std::numeric_limits<std::int8_t>::min();
       signedValue <= std::numeric_limits<std::int8_t>::max(); ++signedValue) {
    for (int unsignedValue = std::numeric_limits<std::uint8_t>::min();
         unsignedValue <= std::numeric_limits<std::uint8_t>::max();
         ++unsignedValue) {
      auto signedOperand = static_cast<std::int8_t>(signedValue);
      auto unsignedOperand = static_cast<std::uint8_t>(unsignedValue);
      checkSigned(overflowingAdd(signedOperand, unsignedOperand),
                  signedValue + unsignedValue);
      checkSigned(overflowingSub(signedOperand, unsignedOperand),
                  signedValue - unsignedValue);
      checkSigned(overflowingMul(signedOperand, unsignedOperand),
                  signedValue * unsignedValue);
      if (unsignedOperand != 0) {
        checkSigned(overflowingDiv(signedOperand, unsignedOperand),
                    signedValue / unsignedValue);
      }
      checkUnsigned(overflowingAdd(unsignedOperand, signedOperand),
                    unsignedValue + signedValue);
      checkUnsigned(overflowingSub(unsignedOperand, signedOperand),
                    unsignedValue - signedValue);
      checkUnsigned(overflowingMul(unsignedOperand, signedOperand),
                    unsignedValue * signedValue);
      if (signedOperand != 0) {
        checkUnsigned(overflowingDiv(unsignedOperand, signedOperand),
                      unsignedValue / signedValue);
      }
    }
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, SupportsRemainderAndEuclideanDivisionFamilies) {
  EXPECT_EQ(checkedRem(-7, 3), -1);
  EXPECT_EQ(checkedDivEuclid(-7, 3), -3);
  EXPECT_EQ(checkedRemEuclid(-7, 3), 2);
  EXPECT_FALSE(checkedRem(std::numeric_limits<int>::min(), -1));

  EXPECT_EQ(overflowingDivEuclid(std::numeric_limits<int>::min(), -1),
            (std::pair{std::numeric_limits<int>::min(), true}));
  EXPECT_EQ(overflowingRem(std::numeric_limits<int>::min(), -1),
            (std::pair{0, true}));
  EXPECT_EQ(wrappingDivEuclid(-7, 3), -3);
  EXPECT_EQ(wrappingRemEuclid(-7, 3), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, SupportsMixedSignArithmeticFamilies) {
  EXPECT_EQ(overflowingAdd(std::int8_t{-100}, std::uint8_t{227}),
            (std::pair{std::int8_t{127}, false}));
  EXPECT_EQ(overflowingAdd(std::int8_t{-100}, std::uint8_t{228}),
            (std::pair{std::int8_t{-128}, true}));
  EXPECT_EQ(overflowingSub(std::int8_t{100}, std::uint8_t{228}),
            (std::pair{std::int8_t{-128}, false}));
  EXPECT_EQ(overflowingSub(std::int8_t{100}, std::uint8_t{229}),
            (std::pair{std::int8_t{127}, true}));

  EXPECT_EQ(overflowingAdd(std::uint8_t{1}, std::int8_t{-2}),
            (std::pair{std::uint8_t{255}, true}));
  EXPECT_EQ(overflowingSub(std::uint8_t{254}, std::int8_t{-2}),
            (std::pair{std::uint8_t{0}, true}));
  EXPECT_EQ(saturatingAdd(std::uint8_t{1}, std::int8_t{-2}), 0);
  EXPECT_EQ(saturatingSub(std::uint8_t{254}, std::int8_t{-2}), 255);
  EXPECT_EQ(wrappingAdd(std::int8_t{127}, std::uint8_t{1}), -128);
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, SupportsUnsignedNegation) {
  EXPECT_EQ(checkedNeg(0U), 0U);
  EXPECT_FALSE(checkedNeg(1U));
  EXPECT_EQ(overflowingNeg(0U), (std::pair{0U, false}));
  EXPECT_EQ(overflowingNeg(1U),
            (std::pair{std::numeric_limits<unsigned int>::max(), true}));
  EXPECT_EQ(wrappingNeg(1U), std::numeric_limits<unsigned int>::max());
}
// cpsc:test:end

// cpsc:test:start
TEST(IntegerOverflow, SupportsNightlyWrappingAndSaturatingOperations) {
  EXPECT_EQ(wrappingNextPowerOfTwo(std::uint8_t{129}), 0);
  EXPECT_EQ(saturatingTruncate<std::int8_t>(1000), 127);
  EXPECT_EQ(saturatingCast(-1), 0U);
  EXPECT_EQ(saturatingCast(std::numeric_limits<unsigned int>::max()),
            std::numeric_limits<int>::max());
}
// cpsc:test:end
