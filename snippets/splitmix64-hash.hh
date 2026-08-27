/* cpsc:meta:start
slug: splitmix64-hash
title: SplitMix64 Hash
shortTitle: SplitMix64 Hash
category: Utility
description: "`std::hash`が衝突耐性やseedのrandomizationを保証しない点を補い、攻撃的なkeyによるunordered containerの衝突集中を起こしにくくする。"
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start SplitMix64HashBuilder / SplitMix64Hash
class SplitMix64HashBuilder {
public:
  using result_type = std::uint64_t;

  [[nodiscard]] static constexpr result_type mix(result_type value) noexcept {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
  }

  [[nodiscard]] static constexpr result_type
  combine(result_type seed, result_type value) noexcept {
    return mix(seed ^
               (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2)));
  }

  inline static const result_type DefaultSeed = []() noexcept {
    result_type time = static_cast<result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    return mix(time + 0x517cc1b727220a95ULL);
  }();

  constexpr explicit SplitMix64HashBuilder(
      result_type seed = DefaultSeed) noexcept
      : state_(seed) {}

  constexpr SplitMix64HashBuilder &append(result_type value) noexcept {
    state_ = combine(state_, value);
    return *this;
  }

  [[nodiscard]] constexpr result_type build() const noexcept {
    return state_;
  }

private:
  result_type state_;
};

template <typename B>
concept HashBuilder = requires(B &builder, std::uint64_t value) {
  { builder.append(value) } -> std::same_as<B &>;
};

inline constexpr std::uint64_t TupleHashTag = 0x243f6a8885a308d3ULL;
inline constexpr std::uint64_t RangeHashTag = 0x13198a2e03707344ULL;

template <HashBuilder B, std::integral T>
constexpr void hashAppend(B &builder, T value) noexcept {
  if constexpr (sizeof(T) <= sizeof(std::uint64_t)) {
    builder.append(static_cast<std::uint64_t>(value));
  } else {
    using U = std::make_unsigned_t<T>;
    U bits = static_cast<U>(value);
    builder.append(static_cast<std::uint64_t>(bits));
    builder.append(static_cast<std::uint64_t>(bits >> 64));
  }
}

template <HashBuilder B, typename T>
  requires std::is_enum_v<T>
constexpr void hashAppend(B &builder, T value) noexcept {
  hashAppend(builder, static_cast<std::underlying_type_t<T>>(value));
}

template <HashBuilder B, typename T>
  requires std::is_pointer_v<T> void hashAppend(B &builder, T value) noexcept {
  builder.append(
      static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(value)));
}

template <HashBuilder B>
constexpr void hashAppend(B &builder, std::nullptr_t) noexcept {
  builder.append(0);
}

template <HashBuilder B, std::floating_point T>
  requires(sizeof(T) == sizeof(std::uint32_t) ||
           sizeof(T) == sizeof(std::uint64_t))
constexpr void hashAppend(B &builder, T value) noexcept {
  if (value == 0) {
    value = static_cast<T>(0);
  }
  if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
    builder.append(std::bit_cast<std::uint32_t>(value));
  } else {
    builder.append(std::bit_cast<std::uint64_t>(value));
  }
}

template <HashBuilder B, typename T>
  requires(!std::ranges::range<T> && requires {
    std::tuple_size<std::remove_cvref_t<T>>::value;
  }) constexpr void hashAppend(B &builder, const T &value) {
  constexpr std::size_t Size = std::tuple_size_v<std::remove_cvref_t<T>>;
  builder.append(TupleHashTag).append(Size);
  std::apply(
      [&builder](const auto &...element) {
        (hashAppend(builder, element), ...);
      },
      value);
}

template <HashBuilder B, std::ranges::input_range R>
constexpr void hashAppend(B &builder, const R &range) {
  builder.append(RangeHashTag);
  std::size_t size = 0;
  for (auto &&element : range) {
    if constexpr (std::same_as<std::ranges::range_value_t<R>, bool>) {
      hashAppend(builder, static_cast<bool>(element));
    } else {
      hashAppend(builder, element);
    }
    ++size;
  }
  builder.append(size);
}

struct SplitMix64Hash {
  using is_transparent = void;

  template <typename T>
  [[nodiscard]] std::size_t operator()(const T &value) const {
    SplitMix64HashBuilder builder;
    hashAppend(builder, value);
    return static_cast<std::size_t>(builder.build());
  }
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics
`SplitMix64HashBuilder`は追加された値を順序付きで内部stateへ混合する。`SplitMix64Hash`は実行ごとのseedからbuilderを作り、`hashAppend`で値を展開して`std::size_t`を返す。

`hashAppend`は`SplitMix64HashBuilder`そのものには依存せず、`HashBuilder`を満たす任意の型を受け取る。builderのprotocolは`append(uint64_t)`がbuilder自身への参照を返すことだけである。

tuple-like型には要素数、rangeには終端の要素数を混合する。rangeとtuple-like型には異なるtagを使い、入れ子の構造や型の境界による自明な衝突を減らす。ハッシュ衝突が存在しないことを保証するものではない。

## Supported Types
| Category | Types | Handling |
| --- | --- | --- |
| Integer | signed・unsigned整数、`bool`、character型 | 64-bitごとに分割して混合 |
| Enum | scoped・unscoped enum | `uint64_t`へ変換 |
| Floating point | `float`、`double` | bit表現を混合し、`-0`は`+0`へ正規化 |
| Pointer | object pointer、`nullptr` | addressを混合 |
| Tuple-like | `std::pair`、`std::tuple` | 各要素を順番に再帰処理 |
| Range | `std::string`、`std::string_view`、`std::vector`、`std::array` | 各要素を順番に再帰処理 |
| Specialized range | `std::vector<bool>`、nested range | proxyを`bool`へ変換して再帰処理 |
| User-defined | custom class・struct | ADLで`hashAppend`を定義 |

`long double`は表現が処理系依存のため対応しない。character pointerは文字列ではなくaddressとして扱う。

GCC・Clangで`__int128_t`と`unsigned __int128`が`std::integral`を満たす場合は、下位・上位64-bitの両方を混合する。

## Requirements
- `SplitMix64Hash`へ渡す型には、直接またはADLによって呼び出せる`hashAppend(builder, value)`が必要
- key equalityで等しい2値は、同じ順序の値を`builder`へ追加しなければならない
- custom `hashAppend`はkey equalityに使わない状態を混合してはならない

## Complexity
| Operation | Time | Space |
| --- | --- | --- |
| Scalar | $O(1)$ | $O(1)$ |
| Tuple-like / range | $O(K)$ | $O(1)$ |

$K$ は要素数。

## Usage
- `std::unordered_set<long long, SplitMix64Hash>` のようにハッシュ型へ指定する
- user-defined typeは、その型と同じnamespaceに`template <HashBuilder B> hashAppend(B &, const T &)`を定義して拡張できる
- プロセスごとにseedが変わるため、ハッシュ値の再現性はない
- `unordered_map` と `unordered_set` の衝突攻撃を起こしにくくするためのもので、暗号学的用途には使用しない
- 同じ型のキーには同じハッシュオブジェクトを使い、等しいキーが常に等しいハッシュ値になるようにする
cpsc:text:end */
