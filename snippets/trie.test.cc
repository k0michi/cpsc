/* cpsc:meta:start
snippet: trie
cpsc:meta:end */

#include "test-support.hh"
#include "trie.hh"

// cpsc:test:start
TEST(Trie, InsertsCountsAndFindsPrefixes) {
  Trie trie;
  trie.insert(std::string("app"));
  trie.insert(std::string("apple"));
  trie.insert(std::string("app"), 2);

  EXPECT_EQ(trie.size(), 4);
  EXPECT_EQ(trie.count(std::string("app")), 3);
  EXPECT_EQ(trie.count(std::string("apple")), 1);
  EXPECT_EQ(trie.count(std::string("ap")), 0);
  EXPECT_EQ(trie.countPrefix(std::string("app")), 4);
  EXPECT_TRUE(trie.contains(std::string("apple")));
  EXPECT_TRUE(trie.containsPrefix(std::string("ap")));
  EXPECT_FALSE(trie.containsPrefix(std::string("aq")));
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, ErasesAtMostTheStoredMultiplicity) {
  Trie trie;
  trie.insert(std::string("a"), 3);
  trie.insert(std::string("ab"), 2);

  EXPECT_EQ(trie.erase(std::string("a"), 2), 2);
  EXPECT_EQ(trie.count(std::string("a")), 1);
  EXPECT_EQ(trie.countPrefix(std::string("a")), 3);
  EXPECT_EQ(trie.erase(std::string("a"), 10), 1);
  EXPECT_EQ(trie.erase(std::string("a")), 0);
  EXPECT_EQ(trie.erase(std::string("missing")), 0);
  EXPECT_EQ(trie.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, HandlesEmptySequences) {
  Trie trie;
  std::string empty;

  trie.insert(empty, 2);
  trie.insert(std::string("a"));

  EXPECT_EQ(trie.count(empty), 2);
  EXPECT_EQ(trie.countPrefix(empty), 3);
  EXPECT_EQ(trie.erase(empty), 1);
  EXPECT_EQ(trie.count(empty), 1);
  EXPECT_EQ(trie.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, SupportsCustomAlphabets) {
  Trie<10, AsciiCharacterToIndex<'0'>, AsciiIndexToCharacter<'0'>> trie;
  trie.insert(std::string("012"));
  trie.insert(std::string("019"));

  EXPECT_TRUE(trie.contains(std::string("012")));
  EXPECT_EQ(trie.countPrefix(std::string("01")), 2);
  EXPECT_EQ(trie.findNode(std::string("a")), decltype(trie)::Null);
  EXPECT_EQ(trie.get<std::string>(1), std::optional<std::string>{"019"});
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, ExposesNodesAndClearsState) {
  Trie trie;
  trie.insert(std::string("ab"));
  std::size_t oldNodeCount = trie.nodeCount();
  std::size_t node = trie.findNode(std::string("ab"));

  EXPECT_EQ(trie.getNode(node).terminalCount, 1);
  EXPECT_EQ(trie[node].prefixCount, 1);
  EXPECT_GT(oldNodeCount, 1);

  trie.clear();

  EXPECT_TRUE(trie.empty());
  EXPECT_EQ(trie.size(), 0);
  EXPECT_EQ(trie.nodeCount(), 1);
  EXPECT_EQ(trie.countPrefix(std::string("")), 0);
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, KeepsAllocatedNodesAfterErasingWords) {
  Trie trie;
  trie.insert(std::string("abc"));
  std::size_t nodeCount = trie.nodeCount();

  EXPECT_EQ(trie.erase(std::string("abc")), 1);
  EXPECT_TRUE(trie.empty());
  EXPECT_EQ(trie.nodeCount(), nodeCount);
  EXPECT_FALSE(trie.containsPrefix(std::string("abc")));
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, CountsLexicographicalBoundsWithMultiplicity) {
  Trie trie;
  trie.insert(std::string(""));
  trie.insert(std::string("app"), 2);
  trie.insert(std::string("apple"));
  trie.insert(std::string("banana"));

  EXPECT_EQ(trie.countLess(std::string("")), 0);
  EXPECT_EQ(trie.countLessEqual(std::string("")), 1);
  EXPECT_EQ(trie.countLess(std::string("app")), 1);
  EXPECT_EQ(trie.countLessEqual(std::string("app")), 3);
  EXPECT_EQ(trie.countLess(std::string("apple")), 3);
  EXPECT_EQ(trie.countLessEqual(std::string("apple")), 4);
  EXPECT_EQ(trie.countLess(std::string("apricot")), 4);
  EXPECT_EQ(trie.countLessEqual(std::string("apricot")), 4);
  EXPECT_EQ(trie.countLess(std::string("banana")), 4);
  EXPECT_EQ(trie.countLessEqual(std::string("banana")), 5);
  EXPECT_EQ(trie.countLess(std::string("z")), 5);
  EXPECT_EQ(trie.countLessEqual(std::string("z")), 5);
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, GetsElementsInLexicographicalOrder) {
  Trie trie;
  trie.insert(std::string(""));
  trie.insert(std::string("a"));
  trie.insert(std::string("ab"), 2);
  trie.insert(std::string("b"));

  EXPECT_EQ(trie.get<std::string>(0), std::optional<std::string>{""});
  EXPECT_EQ(trie.get<std::string>(1), std::optional<std::string>{"a"});
  EXPECT_EQ(trie.get<std::string>(2), std::optional<std::string>{"ab"});
  EXPECT_EQ(trie.get<std::string>(3), std::optional<std::string>{"ab"});
  EXPECT_EQ(trie.get<std::vector<char>>(4),
            (std::optional<std::vector<char>>{{'b'}}));
  EXPECT_EQ(trie.get<std::string>(5), std::nullopt);
}
// cpsc:test:end

// cpsc:test:start
TEST(Trie, FindsLongestCommonPrefixWithStoredElements) {
  Trie trie;
  trie.insert(std::string("apple"));
  trie.insert(std::string("application"));
  trie.insert(std::string("banana"));

  EXPECT_EQ(trie.longestCommonPrefix(std::string("apply")), 4);
  EXPECT_EQ(trie.longestCommonPrefix(std::string("application")), 11);
  EXPECT_EQ(trie.longestCommonPrefix(std::string("band")), 3);
  EXPECT_EQ(trie.longestCommonPrefix(std::string("cat")), 0);
  EXPECT_EQ(trie.longestCommonPrefix(std::string("")), 0);

  trie.erase(std::string("apple"));
  trie.erase(std::string("application"));
  EXPECT_EQ(trie.longestCommonPrefix(std::string("app")), 0);
}
// cpsc:test:end
