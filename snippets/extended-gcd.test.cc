/* cpsc:meta:start
snippet: extended-gcd
cpsc:meta:end */

#include "test-support.hh"
#include "extended-gcd.hh"

// cpsc:test:start
TEST(ExtendedGCD, ComputesGcdAndBezoutCoefficients) {
  for (const auto &[a, b] : std::vector<std::pair<long long, long long>>{
           {30, 18}, {17, 5}, {0, 7}, {7, 0}, {0, 0}}) {
    const auto [gcd, x, y] = extendedGCD(a, b);
    EXPECT_EQ(a * x + b * y, gcd);
    EXPECT_EQ(gcd, std::gcd(a, b));
  }
}
// cpsc:test:end

// cpsc:test:start
TEST(ExtendedGCD, SupportsConstantEvaluation) {
  static_assert([] {
    const auto [gcd, x, y] = extendedGCD(240, 46);
    return gcd == 2 && 240 * x + 46 * y == gcd;
  }());
}
// cpsc:test:end
