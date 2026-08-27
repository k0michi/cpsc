/* cpsc:meta:start
snippet: splitmix64-hash
cpsc:meta:end */

#include <array>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "test-support.hh"
#include "splitmix64-hash.hh"

namespace splitmix64HashTest {
struct Point {
  int x;
  int y;

  bool operator==(const Point &) const = default;
};

template <HashBuilder B> void hashAppend(B &builder, const Point &point) {
  hashAppend(builder, point.x);
  hashAppend(builder, point.y);
}
} // namespace splitmix64HashTest

class RecordingHashBuilder {
public:
  RecordingHashBuilder &append(std::uint64_t value) {
    values.push_back(value);
    return *this;
  }

  std::vector<std::uint64_t> values;
};

// cpsc:test:start
TEST(SplitMix64Hash, StoresSignedAndUnsignedIntegers) {
  std::unordered_set<long long, SplitMix64Hash> signedValues;
  signedValues.insert(0);
  signedValues.insert(-1);
  signedValues.insert(std::numeric_limits<long long>::min());
  signedValues.insert(std::numeric_limits<long long>::max());
  signedValues.insert(-1);

  EXPECT_EQ(signedValues.size(), 4);
  EXPECT_TRUE(signedValues.contains(-1));

  std::unordered_set<unsigned long long, SplitMix64Hash> unsignedValues;
  unsignedValues.insert(0);
  unsignedValues.insert(std::numeric_limits<unsigned long long>::max());

  EXPECT_EQ(unsignedValues.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, MixesBothHalvesOf128BitIntegers) {
#if defined(__SIZEOF_INT128__)
  SplitMix64Hash hash;
  unsigned __int128 low = 1;
  unsigned __int128 high = (static_cast<unsigned __int128>(1) << 64) | 1;

  EXPECT_NE(hash(low), hash(high));
#endif
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, StoresPairsAndTuples) {
  std::unordered_map<std::pair<int, long long>, int, SplitMix64Hash> pairs;
  pairs[{1, -2}] = 3;
  pairs[{1, 2}] = 4;

  EXPECT_EQ(pairs.at({1, -2}), 3);
  EXPECT_EQ(pairs.at({1, 2}), 4);

  using Key = std::tuple<int, unsigned int, long long>;
  std::unordered_set<Key, SplitMix64Hash> tuples;
  tuples.emplace(1, 2U, -3);
  tuples.emplace(1, 2U, -3);
  tuples.emplace(1, 2U, 3);

  EXPECT_EQ(tuples.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, ProducesStableValuesWithinTheProcess) {
  SplitMix64Hash hash;

  EXPECT_EQ(hash(42), hash(42));
  EXPECT_EQ(hash(std::pair{1, 2}), hash(std::pair{1, 2}));
  EXPECT_EQ(hash(std::tuple{1, 2, 3}), hash(std::tuple{1, 2, 3}));
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64HashBuilder, MixesIntegerBitPatterns) {
  static_assert(SplitMix64HashBuilder::mix(0) != SplitMix64HashBuilder::mix(1));
  static_assert(SplitMix64HashBuilder::mix(0) == SplitMix64HashBuilder::mix(0));

  EXPECT_NE(SplitMix64HashBuilder::mix(0),
            SplitMix64HashBuilder::mix(1ULL << 63));
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64HashBuilder, BuildsDeterministicValuesFromExplicitSeed) {
  constexpr auto value = [] {
    SplitMix64HashBuilder builder(123);
    return builder.append(1).append(2).append(3).build();
  }();

  static_assert(value == [] {
    SplitMix64HashBuilder builder(123);
    return builder.append(1).append(2).append(3).build();
  }());
  static_assert(value != [] {
    SplitMix64HashBuilder builder(123);
    return builder.append(3).append(2).append(1).build();
  }());
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, SupportsFloatingPointEnumsAndPointers) {
  enum class Kind : unsigned int { A, B };
  SplitMix64Hash hash;
  int value = 0;

  EXPECT_EQ(hash(0.0), hash(-0.0));
  EXPECT_NE(hash(1.0F), hash(2.0F));
  EXPECT_NE(hash(Kind::A), hash(Kind::B));
  EXPECT_EQ(hash(&value), hash(&value));
  EXPECT_NE(hash(&value), hash(nullptr));
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, SupportsArraysStringsAndHeterogeneousLookup) {
  SplitMix64Hash hash;
  std::array values{1, 2, 3};

  EXPECT_EQ(hash(values), hash(std::vector{1, 2, 3}));

  std::unordered_set<std::string, SplitMix64Hash, std::equal_to<>> strings;
  strings.emplace("cpsc");
  EXPECT_TRUE(strings.contains(std::string_view("cpsc")));
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, SupportsAdlCustomization) {
  std::unordered_set<splitmix64HashTest::Point, SplitMix64Hash> points;
  points.insert({1, 2});
  points.insert({1, 2});
  points.insert({2, 1});

  EXPECT_EQ(points.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(HashAppend, SupportsIndependentBuilders) {
  RecordingHashBuilder builder;

  hashAppend(builder, std::tuple{1, 2U});

  EXPECT_EQ(builder.values,
            (std::vector<std::uint64_t>{TupleHashTag, 2, 1, 2}));
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, StoresVectorsAndNestedVectors) {
  std::unordered_set<std::vector<int>, SplitMix64Hash> vectors;
  vectors.insert(std::vector<int>{});
  vectors.insert({1, 2});
  vectors.insert({2, 1});
  vectors.insert({1, 2});

  EXPECT_EQ(vectors.size(), 3);
  EXPECT_TRUE(vectors.contains({1, 2}));

  using Nested = std::vector<std::vector<std::pair<int, int>>>;
  std::unordered_set<Nested, SplitMix64Hash> nested;
  nested.insert({{{1, 2}}, {{3, 4}, {5, 6}}});
  nested.insert({{{1, 2}}, {{3, 4}, {5, 6}}});
  nested.insert({{{1, 2}}, {{3, 4}}});

  EXPECT_EQ(nested.size(), 2);
}
// cpsc:test:end

// cpsc:test:start
TEST(SplitMix64Hash, StoresBooleanVectors) {
  std::unordered_set<std::vector<bool>, SplitMix64Hash> values;
  values.insert({false, true});
  values.insert({true, false});
  values.insert({false, true});

  EXPECT_EQ(values.size(), 2);
}
// cpsc:test:end
