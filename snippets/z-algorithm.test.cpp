/* cpsc:meta:start
snippet: z-algorithm
cpsc:meta:end */

#include <array>

#include "test-support.hpp"
#include "z-algorithm.cpp"

// cpsc:test:start
TEST(ZAlgorithm, ComputesPrefixMatches) {
  std::string value = "aabcaabxaaaz";

  EXPECT_EQ(z(value),
            (std::vector<std::size_t>{12, 1, 0, 0, 3, 1, 0, 0, 2, 2, 1, 0}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ZAlgorithm, HandlesEmptyAndSingletonRanges) {
  EXPECT_TRUE(z(std::string{}).empty());
  EXPECT_EQ(z(std::string{"x"}), (std::vector<std::size_t>{1}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ZAlgorithm, HandlesFullAndMissingMatches) {
  EXPECT_EQ(z(std::string{"aaaaa"}), (std::vector<std::size_t>{5, 4, 3, 2, 1}));
  EXPECT_EQ(z(std::string{"abcde"}), (std::vector<std::size_t>{5, 0, 0, 0, 0}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ZAlgorithm, SupportsIteratorSubranges) {
  std::string value = "xxababa";

  EXPECT_EQ(z(value.begin() + 2, value.end()),
            (std::vector<std::size_t>{5, 0, 3, 0, 1}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ZAlgorithm, SupportsNonCharacterSequences) {
  std::vector<int> value{1, 2, 1, 2, 1, 3};

  EXPECT_EQ(z(value), (std::vector<std::size_t>{6, 0, 3, 0, 1, 0}));
}
// cpsc:test:end

// cpsc:test:start
TEST(ZAlgorithm, SupportsConstantEvaluation) {
  static_assert([] {
    std::array value{1, 1, 1, 2};
    return z(value) == std::vector<std::size_t>{4, 2, 1, 0};
  }());

  SUCCEED();
}
// cpsc:test:end
