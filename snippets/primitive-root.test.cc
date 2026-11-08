/* cpsc:meta:start
snippet: primitive-root
cpsc:meta:end */

#include "test-support.hh"
#include "primitive-root.hh"

// cpsc:test:start
TEST(PrimitiveRoot, FindsSmallestPrimitiveRoots) {
  EXPECT_EQ(primitiveRoot(2), 1);
  EXPECT_EQ(primitiveRoot(3), 2);
  EXPECT_EQ(primitiveRoot(17), 3);
  EXPECT_EQ(primitiveRoot(97), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(PrimitiveRoot, ComputesNttPrimesAtCompileTime) {
  static_assert(PrimitiveRoot<17> == 3);
  static_assert(PrimitiveRoot<998'244'353> == 3);
  static_assert(PrimitiveRoot<754'974'721> == 11);

  EXPECT_EQ(PrimitiveRoot<469'762'049>, 3);
}
// cpsc:test:end
