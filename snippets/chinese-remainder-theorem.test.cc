/* cpsc:meta:start
snippet: chinese-remainder-theorem
cpsc:meta:end */

#include "test-support.hh"
#include "chinese-remainder-theorem.hh"

// cpsc:test:start
TEST(ChineseRemainderTheorem, MergesCoprimeCongruences) {
  EXPECT_EQ(crt(2, 3, 3, 5),
            (std::optional<std::pair<long long, long long>>{{8, 15}}));
  EXPECT_EQ(crt(-1, 5, 2, 3),
            (std::optional<std::pair<long long, long long>>{{14, 15}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ChineseRemainderTheorem, MergesCompatibleNonCoprimeCongruences) {
  EXPECT_EQ(crt(1, 4, 3, 6),
            (std::optional<std::pair<long long, long long>>{{9, 12}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ChineseRemainderTheorem, RejectsIncompatibleCongruences) {
  EXPECT_EQ(crt(1, 2, 0, 4), std::nullopt);
}
// cpsc:test:end

// cpsc:test:start
TEST(ChineseRemainderTheorem, MergesCongruenceSequence) {
  std::vector<std::pair<long long, long long>> congruences{
      {2, 3}, {3, 5}, {2, 7}};

  EXPECT_EQ(crt(congruences),
            (std::optional<std::pair<long long, long long>>{{23, 105}}));
  EXPECT_EQ(crt(std::vector<std::pair<long long, long long>>{}),
            (std::optional<std::pair<long long, long long>>{{0, 1}}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ChineseRemainderTheorem, SupportsConstantEvaluation) {
  constexpr auto result = crt(2, 3, 3, 5);
  static_assert(result.has_value());
  static_assert(*result == std::pair{8LL, 15LL});
  SUCCEED();
}
// cpsc:test:end

// cpsc:test:start
TEST(ChineseRemainderTheorem, HandlesTrivialAndRepeatedCongruences) {
  EXPECT_EQ(crt(123, 1, -456, 1),
            (std::optional<std::pair<long long, long long>>{{0, 1}}));
  EXPECT_EQ(crt(-1, 7, 6, 7),
            (std::optional<std::pair<long long, long long>>{{6, 7}}));
  EXPECT_EQ(crt(std::vector<std::pair<long long, long long>>{{1, 2}, {0, 4}}),
            std::nullopt);
}
// cpsc:test:end
