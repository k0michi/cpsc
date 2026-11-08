/* cpsc:meta:start
snippet: number-theoretic-transform
cpsc:meta:end */

#include "test-support.hh"
#include "number-theoretic-transform.hh"

using NttMint = ModInt<998'244'353>;

// cpsc:test:start
TEST(NumberTheoreticTransform, RestoresValuesWithInverseTransform) {
  std::vector<NttMint> values{1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<NttMint> original = values;

  numberTheoreticTransform(values, false);
  numberTheoreticTransform(values, true);

  EXPECT_EQ(values, original);
}
// cpsc:test:end

// cpsc:test:start
TEST(NumberTheoreticTransform, HandlesEmptyAndSingleElementSequences) {
  std::vector<NttMint> empty;
  std::vector<NttMint> single{42};

  numberTheoreticTransform(empty, false);
  numberTheoreticTransform(single, false);

  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(single, (std::vector<NttMint>{42}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Convolve, MultipliesSequences) {
  std::vector<NttMint> left{1, 2, 3};
  std::vector<NttMint> right{4, 5, 6, 7};

  EXPECT_EQ(convolve(left, right),
            (std::vector<NttMint>{4, 13, 28, 34, 32, 21}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Convolve, HandlesEmptyAndSingleElementSequences) {
  std::vector<NttMint> empty;
  std::vector<NttMint> values{1, -2, 3};

  EXPECT_TRUE(convolve(empty, values).empty());
  EXPECT_EQ(convolve(std::vector<NttMint>{5}, values),
            (std::vector<NttMint>{5, -10, 15}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Convolve, WorksWithAnotherNttPrime) {
  using Mint = ModInt<17>;
  std::vector<Mint> left{8, 9, 10, 11};
  std::vector<Mint> right{12, 13, 14};

  EXPECT_EQ(convolve(left, right), (std::vector<Mint>{11, 8, 9, 14, 11, 1}));
}
// cpsc:test:end

// cpsc:test:start
TEST(Convolve, AllowsExplicitPrimitiveRoot) {
  std::vector<NttMint> left{1, 2};
  std::vector<NttMint> right{3, 4};

  EXPECT_EQ((convolve<998'244'353, 3>(left, right)),
            (std::vector<NttMint>{3, 10, 8}));
}
// cpsc:test:end
