/* cpsc:meta:start
slug: int128
title: 128-bit Integer
shortTitle: Int128
category: Math
description: ネイティブ128-bit整数がない環境でも使える符号付き・符号なし128-bit整数。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

#include <cassert>
#include <compare>
#include <concepts>
#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

// cpsc:subsnippet:start UInt128 / Int128
struct UInt128 {
private:
  std::uint64_t high_ = 0;
  std::uint64_t low_ = 0;

  [[nodiscard]] static constexpr std::pair<UInt128, UInt128>
  divide(UInt128 dividend, UInt128 divisor) {
    assert(divisor != UInt128{});
#if defined(__SIZEOF_INT128__) && !defined(INT128_FORCE_PORTABLE)
    using Native = unsigned __int128;
    Native left = (static_cast<Native>(dividend.high_) << 64) | dividend.low_;
    Native right = (static_cast<Native>(divisor.high_) << 64) | divisor.low_;
    Native quotient = left / right;
    Native remainder = left % right;
    return {{static_cast<std::uint64_t>(quotient >> 64),
             static_cast<std::uint64_t>(quotient)},
            {static_cast<std::uint64_t>(remainder >> 64),
             static_cast<std::uint64_t>(remainder)}};
#else
    UInt128 quotient;
    UInt128 remainder;
    for (int bit = 127; bit >= 0; --bit) {
      remainder <<= 1;
      remainder.low_ |= static_cast<std::uint64_t>((dividend >> bit).low_ & 1);
      if (remainder >= divisor) {
        remainder -= divisor;
        quotient |= UInt128{1} << bit;
      }
    }
    return {quotient, remainder};
#endif
  }

public:
  constexpr UInt128() = default;
  constexpr UInt128(std::uint64_t high, std::uint64_t low)
      : high_(high), low_(low) {}

  template <std::integral T> constexpr UInt128(T value) {
    low_ = static_cast<std::uint64_t>(value);
    if constexpr (std::signed_integral<T>) {
      high_ = value < 0 ? std::numeric_limits<std::uint64_t>::max() : 0;
    }
  }

#if defined(__SIZEOF_INT128__)
  constexpr UInt128(unsigned __int128 value)
      : high_(static_cast<std::uint64_t>(value >> 64)),
        low_(static_cast<std::uint64_t>(value)) {}
  constexpr UInt128(__int128 value)
      : UInt128(static_cast<unsigned __int128>(value)) {}
#endif

  [[nodiscard]] constexpr std::uint64_t high() const {
    return high_;
  }
  [[nodiscard]] constexpr std::uint64_t low() const {
    return low_;
  }

  template <std::integral T> explicit constexpr operator T() const {
    return static_cast<T>(low_);
  }
  explicit constexpr operator bool() const {
    return high_ != 0 || low_ != 0;
  }
#if defined(__SIZEOF_INT128__)
  explicit constexpr operator unsigned __int128() const {
    return (static_cast<unsigned __int128>(high_) << 64) | low_;
  }
  explicit constexpr operator __int128() const {
    return static_cast<__int128>(static_cast<unsigned __int128>(*this));
  }
#endif

  constexpr UInt128 &operator+=(UInt128 right) {
    std::uint64_t oldLow = low_;
    low_ += right.low_;
    high_ += right.high_ + static_cast<std::uint64_t>(low_ < oldLow);
    return *this;
  }
  constexpr UInt128 &operator-=(UInt128 right) {
    std::uint64_t oldLow = low_;
    low_ -= right.low_;
    high_ -= right.high_ + static_cast<std::uint64_t>(oldLow < right.low_);
    return *this;
  }
  constexpr UInt128 &operator*=(UInt128 right) {
#if defined(__SIZEOF_INT128__) && !defined(INT128_FORCE_PORTABLE)
    *this = UInt128(static_cast<unsigned __int128>(*this) *
                    static_cast<unsigned __int128>(right));
#else
    UInt128 left = *this;
    UInt128 product;
    while (right != UInt128{}) {
      if ((right.low_ & 1) != 0) {
        product += left;
      }
      left <<= 1;
      right >>= 1;
    }
    *this = product;
#endif
    return *this;
  }
  constexpr UInt128 &operator/=(UInt128 right) {
    return *this = divide(*this, right).first;
  }
  constexpr UInt128 &operator%=(UInt128 right) {
    return *this = divide(*this, right).second;
  }
  constexpr UInt128 &operator&=(UInt128 right) {
    high_ &= right.high_;
    low_ &= right.low_;
    return *this;
  }
  constexpr UInt128 &operator|=(UInt128 right) {
    high_ |= right.high_;
    low_ |= right.low_;
    return *this;
  }
  constexpr UInt128 &operator^=(UInt128 right) {
    high_ ^= right.high_;
    low_ ^= right.low_;
    return *this;
  }
  constexpr UInt128 &operator<<=(int shift) {
    assert(shift >= 0);
    if (shift >= 128) {
      return high_ = low_ = 0, *this;
    }
    if (shift >= 64) {
      high_ = low_ << (shift - 64);
      low_ = 0;
    } else if (shift != 0) {
      high_ = (high_ << shift) | (low_ >> (64 - shift));
      low_ <<= shift;
    }
    return *this;
  }
  constexpr UInt128 &operator>>=(int shift) {
    assert(shift >= 0);
    if (shift >= 128) {
      return high_ = low_ = 0, *this;
    }
    if (shift >= 64) {
      low_ = high_ >> (shift - 64);
      high_ = 0;
    } else if (shift != 0) {
      low_ = (low_ >> shift) | (high_ << (64 - shift));
      high_ >>= shift;
    }
    return *this;
  }

  [[nodiscard]] constexpr UInt128 operator+() const {
    return *this;
  }
  [[nodiscard]] constexpr UInt128 operator-() const {
    return ~*this + 1;
  }
  [[nodiscard]] constexpr UInt128 operator~() const {
    return {~high_, ~low_};
  }
  constexpr UInt128 &operator++() {
    return *this += 1;
  }
  constexpr UInt128 operator++(int) {
    auto old = *this;
    ++*this;
    return old;
  }
  constexpr UInt128 &operator--() {
    return *this -= 1;
  }
  constexpr UInt128 operator--(int) {
    auto old = *this;
    --*this;
    return old;
  }

  friend constexpr bool operator==(UInt128, UInt128) = default;
  friend constexpr std::strong_ordering operator<=>(UInt128 left,
                                                    UInt128 right) {
    if (auto order = left.high_ <=> right.high_; order != 0) {
      return order;
    }
    return left.low_ <=> right.low_;
  }

  friend constexpr UInt128 operator+(UInt128 left, UInt128 right) {
    return left += right;
  }
  friend constexpr UInt128 operator-(UInt128 left, UInt128 right) {
    return left -= right;
  }
  friend constexpr UInt128 operator*(UInt128 left, UInt128 right) {
    return left *= right;
  }
  friend constexpr UInt128 operator/(UInt128 left, UInt128 right) {
    return left /= right;
  }
  friend constexpr UInt128 operator%(UInt128 left, UInt128 right) {
    return left %= right;
  }
  friend constexpr UInt128 operator&(UInt128 left, UInt128 right) {
    return left &= right;
  }
  friend constexpr UInt128 operator|(UInt128 left, UInt128 right) {
    return left |= right;
  }
  friend constexpr UInt128 operator^(UInt128 left, UInt128 right) {
    return left ^= right;
  }
  friend constexpr UInt128 operator<<(UInt128 value, int shift) {
    return value <<= shift;
  }
  friend constexpr UInt128 operator>>(UInt128 value, int shift) {
    return value >>= shift;
  }

  [[nodiscard]] std::string toString() const {
    if (*this == UInt128{}) {
      return "0";
    }
    UInt128 value = *this;
    std::string result;
    do {
      auto [quotient, remainder] = divide(value, 10);
      result.push_back(static_cast<char>('0' + remainder.low_));
      value = quotient;
    } while (value != UInt128{});
    for (std::size_t left = 0, right = result.size() - 1; left < right;
         ++left, --right) {
      char temporary = result[left];
      result[left] = result[right];
      result[right] = temporary;
    }
    return result;
  }
};

struct Int128 {
private:
  UInt128 bits_;

  [[nodiscard]] constexpr bool negative() const {
    return (bits_.high() >> 63) != 0;
  }
  [[nodiscard]] constexpr UInt128 magnitude() const {
    return negative() ? -bits_ : bits_;
  }

public:
  constexpr Int128() = default;
  template <std::integral T> constexpr Int128(T value) : bits_(value) {}
  explicit constexpr Int128(UInt128 bits) : bits_(bits) {}
#if defined(__SIZEOF_INT128__)
  constexpr Int128(__int128 value)
      : bits_(static_cast<unsigned __int128>(value)) {}
  explicit constexpr Int128(unsigned __int128 value) : bits_(value) {}
#endif

  [[nodiscard]] constexpr std::uint64_t high() const {
    return bits_.high();
  }
  [[nodiscard]] constexpr std::uint64_t low() const {
    return bits_.low();
  }
  [[nodiscard]] constexpr UInt128 unsignedBits() const {
    return bits_;
  }

  template <std::integral T> explicit constexpr operator T() const {
    return static_cast<T>(bits_);
  }
  explicit constexpr operator bool() const {
    return static_cast<bool>(bits_);
  }
#if defined(__SIZEOF_INT128__)
  explicit constexpr operator __int128() const {
    return static_cast<__int128>(static_cast<unsigned __int128>(bits_));
  }
  explicit constexpr operator unsigned __int128() const {
    return static_cast<unsigned __int128>(bits_);
  }
#endif

  constexpr Int128 &operator+=(Int128 right) {
    bits_ += right.bits_;
    return *this;
  }
  constexpr Int128 &operator-=(Int128 right) {
    bits_ -= right.bits_;
    return *this;
  }
  constexpr Int128 &operator*=(Int128 right) {
    bits_ *= right.bits_;
    return *this;
  }
  constexpr Int128 &operator/=(Int128 right) {
    bool resultNegative = negative() != right.negative();
    UInt128 quotient = magnitude() / right.magnitude();
    bits_ = resultNegative ? -quotient : quotient;
    return *this;
  }
  constexpr Int128 &operator%=(Int128 right) {
    bool resultNegative = negative();
    UInt128 remainder = magnitude() % right.magnitude();
    bits_ = resultNegative ? -remainder : remainder;
    return *this;
  }
  constexpr Int128 &operator&=(Int128 right) {
    bits_ &= right.bits_;
    return *this;
  }
  constexpr Int128 &operator|=(Int128 right) {
    bits_ |= right.bits_;
    return *this;
  }
  constexpr Int128 &operator^=(Int128 right) {
    bits_ ^= right.bits_;
    return *this;
  }
  constexpr Int128 &operator<<=(int shift) {
    bits_ <<= shift;
    return *this;
  }
  constexpr Int128 &operator>>=(int shift) {
    assert(shift >= 0);
    bool sign = negative();
    if (shift >= 128) {
      bits_ = sign ? UInt128{~std::uint64_t{}, ~std::uint64_t{}} : UInt128{};
    } else if (sign && shift != 0) {
      bits_ >>= shift;
      bits_ |= ~UInt128{} << (128 - shift);
    } else {
      bits_ >>= shift;
    }
    return *this;
  }
  [[nodiscard]] constexpr Int128 operator+() const {
    return *this;
  }
  [[nodiscard]] constexpr Int128 operator-() const {
    return Int128(-bits_);
  }
  [[nodiscard]] constexpr Int128 operator~() const {
    return Int128(~bits_);
  }
  constexpr Int128 &operator++() {
    return *this += 1;
  }
  constexpr Int128 operator++(int) {
    auto old = *this;
    ++*this;
    return old;
  }
  constexpr Int128 &operator--() {
    return *this -= 1;
  }
  constexpr Int128 operator--(int) {
    auto old = *this;
    --*this;
    return old;
  }

  friend constexpr bool operator==(Int128, Int128) = default;
  friend constexpr std::strong_ordering operator<=>(Int128 left, Int128 right) {
    if (left.negative() != right.negative()) {
      return left.negative() ? std::strong_ordering::less
                             : std::strong_ordering::greater;
    }
    return left.bits_ <=> right.bits_;
  }
  friend constexpr Int128 operator+(Int128 left, Int128 right) {
    return left += right;
  }
  friend constexpr Int128 operator-(Int128 left, Int128 right) {
    return left -= right;
  }
  friend constexpr Int128 operator*(Int128 left, Int128 right) {
    return left *= right;
  }
  friend constexpr Int128 operator/(Int128 left, Int128 right) {
    return left /= right;
  }
  friend constexpr Int128 operator%(Int128 left, Int128 right) {
    return left %= right;
  }
  friend constexpr Int128 operator&(Int128 left, Int128 right) {
    return left &= right;
  }
  friend constexpr Int128 operator|(Int128 left, Int128 right) {
    return left |= right;
  }
  friend constexpr Int128 operator^(Int128 left, Int128 right) {
    return left ^= right;
  }
  friend constexpr Int128 operator<<(Int128 value, int shift) {
    return value <<= shift;
  }
  friend constexpr Int128 operator>>(Int128 value, int shift) {
    return value >>= shift;
  }

  [[nodiscard]] std::string toString() const {
    return negative() ? "-" + magnitude().toString() : bits_.toString();
  }
};

inline std::ostream &operator<<(std::ostream &stream, UInt128 value) {
  return stream << value.toString();
}
inline std::ostream &operator<<(std::ostream &stream, Int128 value) {
  return stream << value.toString();
}

inline std::istream &operator>>(std::istream &stream, UInt128 &value) {
  std::string text;
  stream >> text;
  UInt128 parsed;
  for (char digit : text) {
    if (digit < '0' || digit > '9') {
      stream.setstate(std::ios::failbit);
      return stream;
    }
    parsed = parsed * 10 + (digit - '0');
  }
  if (!text.empty()) {
    value = parsed;
  } else {
    stream.setstate(std::ios::failbit);
  }
  return stream;
}

inline std::istream &operator>>(std::istream &stream, Int128 &value) {
  std::string text;
  stream >> text;
  bool negative = !text.empty() && text.front() == '-';
  std::size_t offset = negative ? 1 : 0;
  UInt128 parsed;
  if (offset == text.size()) {
    stream.setstate(std::ios::failbit);
  }
  for (std::size_t i = offset; i < text.size() && stream; ++i) {
    if (text[i] < '0' || text[i] > '9') {
      stream.setstate(std::ios::failbit);
      break;
    }
    parsed = parsed * 10 + (text[i] - '0');
  }
  if (stream) {
    value = negative ? -Int128(parsed) : Int128(parsed);
  }
  return stream;
}
namespace std {
template <> class numeric_limits<UInt128> {
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = true;
  static constexpr int radix = 2;
  static constexpr int digits = 128;
  static constexpr int digits10 = 38;

  [[nodiscard]] static constexpr UInt128 min() noexcept {
    return {};
  }
  [[nodiscard]] static constexpr UInt128 lowest() noexcept {
    return {};
  }
  [[nodiscard]] static constexpr UInt128 max() noexcept {
    return {~uint64_t{}, ~uint64_t{}};
  }
};

template <> class numeric_limits<Int128> {
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;
  static constexpr int radix = 2;
  static constexpr int digits = 127;
  static constexpr int digits10 = 38;

  [[nodiscard]] static constexpr Int128 min() noexcept {
    return Int128(UInt128{uint64_t{1} << 63, 0});
  }
  [[nodiscard]] static constexpr Int128 lowest() noexcept {
    return min();
  }
  [[nodiscard]] static constexpr Int128 max() noexcept {
    return Int128(UInt128{(uint64_t{1} << 63) - 1, ~uint64_t{}});
  }
};
} // namespace std
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
すべて$O(1)$。portable版の乗除算は最大128回反復する。

## Usage
- `UInt128`は$0$以上$2^{128}-1$以下、`Int128`は$-2^{127}$以上$2^{127}-1$以下を2の補数で表す
- 四則演算、剰余、比較、bit演算、shift、increment/decrement、整数との変換、stream入出力を提供する
- `__SIZEOF_INT128__`が定義される環境ではネイティブ128-bit演算を利用する
- `INT128_FORCE_PORTABLE`を定義すると、64-bit整数だけのフォールバック実装を利用する
- 組み込み整数と同様にoverflowは$2^{128}$を法としてwrapする。0除算と負のshift幅はassertする
cpsc:text:end */
