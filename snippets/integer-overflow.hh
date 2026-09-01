/* cpsc:meta:start
slug: integer-overflow
title: Checked / Overflowing / Saturating / Wrapping Integer Arithmetic
shortTitle: Integer Overflow
category: Math
description: Rust風のoverflow付き整数演算と、C++23向けの安全な整数castを提供する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

// cpsc:subsnippet:start Integer Overflow Arithmetic
template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr std::pair<T, bool> overflowingAdd(T left,
                                                          U right) noexcept {
  using Unsigned = std::make_unsigned_t<T>;
  Unsigned bits = static_cast<Unsigned>(left) + static_cast<Unsigned>(right);
  T result = static_cast<T>(bits);
  bool overflow;
  if constexpr (std::same_as<T, U> && std::unsigned_integral<T>) {
    overflow = left > std::numeric_limits<T>::max() - right;
  } else if constexpr (std::same_as<T, U>) {
    overflow = (right > 0 && left > std::numeric_limits<T>::max() - right) ||
               (right < 0 && left < std::numeric_limits<T>::lowest() - right);
  } else if constexpr (std::signed_integral<T>) {
    Unsigned room = static_cast<Unsigned>(std::numeric_limits<T>::max()) +
                    (Unsigned{} - static_cast<Unsigned>(left));
    overflow = right > room;
  } else if (right >= 0) {
    Unsigned amount = static_cast<Unsigned>(right);
    overflow = amount > std::numeric_limits<T>::max() - left;
  } else {
    Unsigned amount = Unsigned{} - static_cast<Unsigned>(right);
    overflow = amount > left;
  }
  return {result, overflow};
}

template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr std::pair<T, bool> overflowingSub(T left,
                                                          U right) noexcept {
  using Unsigned = std::make_unsigned_t<T>;
  Unsigned bits = static_cast<Unsigned>(left) - static_cast<Unsigned>(right);
  T result = static_cast<T>(bits);
  bool overflow;
  if constexpr (std::same_as<T, U> && std::unsigned_integral<T>) {
    overflow = left < right;
  } else if constexpr (std::same_as<T, U>) {
    overflow = (right > 0 && left < std::numeric_limits<T>::lowest() + right) ||
               (right < 0 && left > std::numeric_limits<T>::max() + right);
  } else if constexpr (std::signed_integral<T>) {
    Unsigned room = static_cast<Unsigned>(left) -
                    static_cast<Unsigned>(std::numeric_limits<T>::lowest());
    overflow = right > room;
  } else if (right >= 0) {
    Unsigned amount = static_cast<Unsigned>(right);
    overflow = amount > left;
  } else {
    Unsigned amount = Unsigned{} - static_cast<Unsigned>(right);
    overflow = amount > std::numeric_limits<T>::max() - left;
  }
  return {result, overflow};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingMul(T left,
                                                          T right) noexcept {
  using U = std::make_unsigned_t<T>;
  U bits = static_cast<U>(left) * static_cast<U>(right);
  T result = static_cast<T>(bits);
  bool overflow = false;
  if constexpr (std::unsigned_integral<T>) {
    overflow = right != 0 && left > std::numeric_limits<T>::max() / right;
  } else if (left > 0) {
    overflow = (right > 0 && left > std::numeric_limits<T>::max() / right) ||
               (right < 0 && right < std::numeric_limits<T>::lowest() / left);
  } else if (left < 0) {
    overflow = (right > 0 && left < std::numeric_limits<T>::lowest() / right) ||
               (right < 0 && right < std::numeric_limits<T>::max() / left);
  }
  return {result, overflow};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingDiv(T left, T right) {
  assert(right != 0);
  if constexpr (std::signed_integral<T>) {
    if (left == std::numeric_limits<T>::lowest() && right == T{-1}) {
      return {left, true};
    }
  }
  return {static_cast<T>(left / right), false};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingNeg(T value) noexcept {
  using U = std::make_unsigned_t<T>;
  U bits = U{} - static_cast<U>(value);
  T result = static_cast<T>(bits);
  if constexpr (std::unsigned_integral<T>) {
    return {result, value != 0};
  }
  return {result, value == std::numeric_limits<T>::lowest()};
}

template <std::signed_integral T>
[[nodiscard]] constexpr std::pair<T, bool> overflowingAbs(T value) noexcept {
  return value < 0 ? overflowingNeg(value) : std::pair<T, bool>{value, false};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool>
overflowingPow(T base, std::uint32_t exponent) noexcept {
  T result = 1;
  bool overflow = false;
  while (exponent != 0) {
    if ((exponent & 1U) != 0) {
      auto [product, didOverflow] = overflowingMul(result, base);
      result = product;
      overflow |= didOverflow;
    }
    exponent >>= 1U;
    if (exponent != 0) {
      auto [square, didOverflow] = overflowingMul(base, base);
      base = square;
      overflow |= didOverflow;
    }
  }
  return {result, overflow};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool>
overflowingShl(T value, unsigned int shift) noexcept {
  constexpr unsigned int width =
      std::numeric_limits<std::make_unsigned_t<T>>::digits;
  using U = std::make_unsigned_t<T>;
  U bits = static_cast<U>(value) << (shift % width);
  T result = static_cast<T>(bits);
  return {result, shift >= width};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool>
overflowingShr(T value, unsigned int shift) noexcept {
  constexpr unsigned int width =
      std::numeric_limits<std::make_unsigned_t<T>>::digits;
  return {static_cast<T>(value >> (shift % width)), shift >= width};
}

template <std::integral To, std::integral From>
  requires(!std::same_as<std::remove_cv_t<To>, bool> &&
           !std::same_as<std::remove_cv_t<From>, bool>)
[[nodiscard]] constexpr std::pair<To, bool>
overflowingCast(From value) noexcept {
  using U = std::make_unsigned_t<To>;
  U bits = static_cast<U>(value);
  To result = static_cast<To>(bits);
  return {result, !std::in_range<To>(value)};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingRem(T left, T right) {
  assert(right != 0);
  if constexpr (std::signed_integral<T>) {
    if (left == std::numeric_limits<T>::lowest() && right == T{-1}) {
      return {0, true};
    }
  }
  return {static_cast<T>(left % right), false};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingDivEuclid(T left,
                                                                T right) {
  auto [quotient, overflow] = overflowingDiv(left, right);
  if constexpr (std::signed_integral<T>) {
    if (!overflow) {
      T remainder = static_cast<T>(left % right);
      if (remainder < 0) {
        quotient += right > 0 ? T{-1} : T{1};
      }
    }
  }
  return {quotient, overflow};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::pair<T, bool> overflowingRemEuclid(T left,
                                                                T right) {
  auto [remainder, overflow] = overflowingRem(left, right);
  if constexpr (std::signed_integral<T>) {
    if (!overflow && remainder < 0) {
      remainder = right > 0 ? static_cast<T>(remainder + right)
                            : static_cast<T>(remainder - right);
    }
  }
  return {remainder, overflow};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedAdd(T left, T right) noexcept {
  auto [value, overflow] = overflowingAdd(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedSub(T left, T right) noexcept {
  auto [value, overflow] = overflowingSub(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedMul(T left, T right) noexcept {
  auto [value, overflow] = overflowingMul(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedDiv(T left, T right) noexcept {
  if (right == 0) {
    return std::nullopt;
  }
  auto [value, overflow] = overflowingDiv(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedNeg(T value) noexcept {
  auto [result, overflow] = overflowingNeg(value);
  return overflow ? std::nullopt : std::optional<T>{result};
}

template <std::signed_integral T>
[[nodiscard]] constexpr std::optional<T> checkedAbs(T value) noexcept {
  auto [result, overflow] = overflowingAbs(value);
  return overflow ? std::nullopt : std::optional<T>{result};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T>
checkedPow(T base, std::uint32_t exponent) noexcept {
  auto [value, overflow] = overflowingPow(base, exponent);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T>
checkedShl(T value, unsigned int shift) noexcept {
  constexpr unsigned int width =
      std::numeric_limits<std::make_unsigned_t<T>>::digits;
  if (shift >= width) {
    return std::nullopt;
  }
  return overflowingShl(value, shift).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T>
checkedShr(T value, unsigned int shift) noexcept {
  constexpr unsigned int width =
      std::numeric_limits<std::make_unsigned_t<T>>::digits;
  if (shift >= width) {
    return std::nullopt;
  }
  return overflowingShr(value, shift).first;
}

template <std::integral To, std::integral From>
  requires(!std::same_as<std::remove_cv_t<To>, bool> &&
           !std::same_as<std::remove_cv_t<From>, bool>)
[[nodiscard]] constexpr std::optional<To> checkedCast(From value) noexcept {
  auto [result, overflow] = overflowingCast<To>(value);
  return overflow ? std::nullopt : std::optional<To>{result};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedRem(T left, T right) noexcept {
  if (right == 0) {
    return std::nullopt;
  }
  auto [value, overflow] = overflowingRem(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedDivEuclid(T left,
                                                          T right) noexcept {
  if (right == 0) {
    return std::nullopt;
  }
  auto [value, overflow] = overflowingDivEuclid(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::optional<T> checkedRemEuclid(T left,
                                                          T right) noexcept {
  if (right == 0) {
    return std::nullopt;
  }
  auto [value, overflow] = overflowingRemEuclid(left, right);
  return overflow ? std::nullopt : std::optional<T>{value};
}

template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr T wrappingAdd(T left, U right) noexcept {
  return overflowingAdd(left, right).first;
}

template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr T wrappingSub(T left, U right) noexcept {
  return overflowingSub(left, right).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingMul(T left, T right) noexcept {
  return overflowingMul(left, right).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingDiv(T left, T right) {
  return overflowingDiv(left, right).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingNeg(T value) noexcept {
  return overflowingNeg(value).first;
}

template <std::signed_integral T>
[[nodiscard]] constexpr T wrappingAbs(T value) noexcept {
  return overflowingAbs(value).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingPow(T base, std::uint32_t exponent) noexcept {
  return overflowingPow(base, exponent).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingShl(T value, unsigned int shift) noexcept {
  return overflowingShl(value, shift).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingShr(T value, unsigned int shift) noexcept {
  return overflowingShr(value, shift).first;
}

template <std::integral To, std::integral From>
  requires(!std::same_as<std::remove_cv_t<To>, bool> &&
           !std::same_as<std::remove_cv_t<From>, bool>)
[[nodiscard]] constexpr To wrappingCast(From value) noexcept {
  return overflowingCast<To>(value).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingRem(T left, T right) {
  return overflowingRem(left, right).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingDivEuclid(T left, T right) {
  return overflowingDivEuclid(left, right).first;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T wrappingRemEuclid(T left, T right) {
  return overflowingRemEuclid(left, right).first;
}

template <std::unsigned_integral T>
  requires(!std::same_as<T, bool>)
[[nodiscard]] constexpr T wrappingNextPowerOfTwo(T value) noexcept {
  if (value <= 1) {
    return 1;
  }
  --value;
  for (int shift = 1; shift < std::numeric_limits<T>::digits; shift *= 2) {
    value = static_cast<T>(value | overflowingShr(value, shift).first);
  }
  return overflowingAdd(value, T{1}).first;
}

template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr T saturatingAdd(T left, U right) noexcept {
  auto [value, overflow] = overflowingAdd(left, right);
  if (!overflow) {
    return value;
  }
  if constexpr (std::signed_integral<T>) {
    if constexpr (std::unsigned_integral<U>) {
      return std::numeric_limits<T>::max();
    }
    return right < 0 ? std::numeric_limits<T>::lowest()
                     : std::numeric_limits<T>::max();
  } else {
    if constexpr (std::signed_integral<U>) {
      return right < 0 ? T{} : std::numeric_limits<T>::max();
    }
    return std::numeric_limits<T>::max();
  }
}

template <std::integral T, std::integral U>
  requires(!std::same_as<std::remove_cv_t<T>, bool> &&
           !std::same_as<std::remove_cv_t<U>, bool> &&
           (std::same_as<T, U> || std::same_as<U, std::make_unsigned_t<T>> ||
            std::same_as<U, std::make_signed_t<T>>))
[[nodiscard]] constexpr T saturatingSub(T left, U right) noexcept {
  auto [value, overflow] = overflowingSub(left, right);
  if (!overflow) {
    return value;
  }
  if constexpr (std::signed_integral<T>) {
    if constexpr (std::unsigned_integral<U>) {
      return std::numeric_limits<T>::lowest();
    }
    return right < 0 ? std::numeric_limits<T>::max()
                     : std::numeric_limits<T>::lowest();
  } else {
    if constexpr (std::signed_integral<U>) {
      return right < 0 ? std::numeric_limits<T>::max() : T{};
    }
    return T{};
  }
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T saturatingMul(T left, T right) noexcept {
  auto [value, overflow] = overflowingMul(left, right);
  if (!overflow) {
    return value;
  }
  if constexpr (std::unsigned_integral<T>) {
    return std::numeric_limits<T>::max();
  }
  return (left < 0) == (right < 0) ? std::numeric_limits<T>::max()
                                   : std::numeric_limits<T>::lowest();
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T saturatingDiv(T left, T right) {
  assert(right != 0);
  auto [value, overflow] = overflowingDiv(left, right);
  return overflow ? std::numeric_limits<T>::max() : value;
}

template <std::signed_integral T>
[[nodiscard]] constexpr T saturatingNeg(T value) noexcept {
  auto [result, overflow] = overflowingNeg(value);
  return overflow ? std::numeric_limits<T>::max() : result;
}

template <std::signed_integral T>
[[nodiscard]] constexpr T saturatingAbs(T value) noexcept {
  auto [result, overflow] = overflowingAbs(value);
  return overflow ? std::numeric_limits<T>::max() : result;
}

template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr T saturatingPow(T base,
                                        std::uint32_t exponent) noexcept {
  auto [value, overflow] = overflowingPow(base, exponent);
  if (!overflow) {
    return value;
  }
  if constexpr (std::unsigned_integral<T>) {
    return std::numeric_limits<T>::max();
  }
  return base < 0 && (exponent & 1U) != 0 ? std::numeric_limits<T>::lowest()
                                          : std::numeric_limits<T>::max();
}

template <typename To = void, std::integral From>
  requires(!std::same_as<std::remove_cv_t<From>, bool> &&
           (std::same_as<To, void> ||
            (std::integral<To> && !std::same_as<std::remove_cv_t<To>, bool>)))
[[nodiscard]] constexpr auto saturatingCast(From value) noexcept {
  using Result = std::conditional_t<
      std::same_as<To, void>,
      std::conditional_t<std::signed_integral<From>, std::make_unsigned_t<From>,
                         std::make_signed_t<From>>,
      To>;
  auto [result, overflow] = overflowingCast<Result>(value);
  if (!overflow) {
    return result;
  }
  if constexpr (std::signed_integral<From>) {
    if (value < 0) {
      return std::numeric_limits<Result>::lowest();
    }
  }
  return std::numeric_limits<Result>::max();
}

template <std::integral To, std::integral From>
  requires(!std::same_as<std::remove_cv_t<To>, bool> &&
           !std::same_as<std::remove_cv_t<From>, bool>)
[[nodiscard]] constexpr To saturatingTruncate(From value) noexcept {
  static_assert(sizeof(To) <= sizeof(From));
  return saturatingCast<To>(value);
}

// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics
- 関数名はlower camel caseとし、戻り値と挙動はRustの整数APIに合わせている
- `checkedAdd`などのchecked系はoverflowや不正なshift幅、0除算で`std::nullopt`を返す
- `overflowingAdd`などのoverflowing系は`{wrapした値, overflowしたか}`を返す
- overflowing系をprimitiveとし、checked・saturating・wrapping系はその結果を利用する
- `saturatingAdd`などのsaturating系は表現可能範囲の端で飽和する
- `wrappingAdd`などのwrapping系は$2^N$を法としてwrapする
- `checkedCast<To>(value)`などのcast系は同じ4種類の方針で標準整数型間を変換する
- `overflowingDiv`、`saturatingDiv`、`wrappingDiv`の除数は0以外であること
- `bool`は対象外

## Complexity
pow系は$O(\log n)$、それ以外は$O(1)$。
cpsc:text:end */
