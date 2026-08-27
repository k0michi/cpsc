/* cpsc:meta:start
snippet: ascii-case-conversion
cpsc:meta:end */

#include "test-support.hh"
#include "ascii-case-conversion.hh"

// cpsc:test:start
TEST(AsciiCaseConversion, ConvertsCharacters) {
  static_assert(toLowerCase('A') == 'a');
  static_assert(toLowerCase('Z') == 'z');
  static_assert(toUpperCase('a') == 'A');
  static_assert(toUpperCase('z') == 'Z');

  EXPECT_EQ(toLowerCase('C'), 'c');
  EXPECT_EQ(toUpperCase('q'), 'Q');
}
// cpsc:test:end

// cpsc:test:start
TEST(AsciiCaseConversion, PreservesNonMatchingBytes) {
  EXPECT_EQ(toLowerCase('a'), 'a');
  EXPECT_EQ(toUpperCase('A'), 'A');
  EXPECT_EQ(toLowerCase('0'), '0');
  EXPECT_EQ(toUpperCase('-'), '-');
  EXPECT_EQ(toLowerCase(static_cast<char>(0xff)), static_cast<char>(0xff));
}
// cpsc:test:end

// cpsc:test:start
TEST(AsciiCaseConversion, ReturnsConvertedCopies) {
  EXPECT_EQ(toLowerCase("Hello, World!"), "hello, world!");
  EXPECT_EQ(toUpperCase("Hello, World!"), "HELLO, WORLD!");
  EXPECT_EQ(toLowerCase(""), "");
  EXPECT_EQ(toUpperCase(""), "");
}
// cpsc:test:end

// cpsc:test:start
TEST(AsciiCaseConversion, ConvertsStringsInPlace) {
  std::string lower = "AbC-123";
  std::string upper = "aBc-123";

  toLowerCaseInPlace(lower);
  toUpperCaseInPlace(upper);

  EXPECT_EQ(lower, "abc-123");
  EXPECT_EQ(upper, "ABC-123");
}
// cpsc:test:end

// cpsc:test:start
TEST(AsciiCaseConversion, SupportsConstantEvaluation) {
  static_assert(toLowerCase(std::string_view("AbC")) == "abc");
  static_assert(toUpperCase(std::string_view("aBc")) == "ABC");

  SUCCEED();
}
// cpsc:test:end
