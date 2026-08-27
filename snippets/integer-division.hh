/* cpsc:meta:start
slug: integer-division
title: Floor / Ceil / Round Division
shortTitle: Integer Division
category: Math
description: C++の0方向へ丸める整数除算を、floor、ceil、最寄りの整数へ補正する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start floorDiv
template <std::integral T>
[[nodiscard]] constexpr T floorDiv(T a, T b) noexcept {
  T q = a / b;
  T r = a % b;
  bool opposite = (a < 0) ^ (b < 0);
  return (r != 0 && opposite) ? q - 1 : q;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start floorMod
template <std::integral T>
[[nodiscard]] constexpr T floorMod(T a, T b) noexcept {
  T r = a % b;
  bool opposite = (a < 0) ^ (b < 0);
  return (r != 0 && opposite) ? r + b : r;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilDiv
template <std::integral T>
[[nodiscard]] constexpr T ceilDiv(T a, T b) noexcept {
  T q = a / b;
  T r = a % b;
  bool same = (a < 0) == (b < 0);
  return (r != 0 && same) ? q + 1 : q;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilMod
template <std::integral T>
[[nodiscard]] constexpr T ceilMod(T a, T b) noexcept {
  T r = a % b;
  bool same = (a < 0) == (b < 0);
  return (r != 0 && same) ? r - b : r;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start roundDiv
template <std::integral T>
[[nodiscard]] constexpr T roundDiv(T a, T b) noexcept {
  T quotient = a / b;
  T remainder = a % b;
  using U = std::make_unsigned_t<T>;
  U remainderMagnitude = remainder < 0 ? U{0} - static_cast<U>(remainder)
                                       : static_cast<U>(remainder);
  U divisorMagnitude = b < 0 ? U{0} - static_cast<U>(b) : static_cast<U>(b);

  if (remainderMagnitude >= divisorMagnitude - remainderMagnitude) {
    bool same = (a < 0) == (b < 0);
    return same ? quotient + 1 : quotient - 1;
  }
  return quotient;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start roundMod
template <std::integral T>
[[nodiscard]] constexpr T roundMod(T a, T b) noexcept {
  T remainder = a % b;
  using U = std::make_unsigned_t<T>;
  U remainderMagnitude = remainder < 0 ? U{0} - static_cast<U>(remainder)
                                       : static_cast<U>(remainder);
  U divisorMagnitude = b < 0 ? U{0} - static_cast<U>(b) : static_cast<U>(b);

  if (remainderMagnitude >= divisorMagnitude - remainderMagnitude) {
    bool same = (a < 0) == (b < 0);
    return same ? remainder - b : remainder + b;
  }
  return remainder;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

$$
\begin{aligned}
\operatorname{floorDiv}(a,b) &= \left\lfloor\frac{a}{b}\right\rfloor, \\
\operatorname{floorMod}(a,b) &= a-b\operatorname{floorDiv}(a,b), \\
\operatorname{ceilDiv}(a,b) &= \left\lceil\frac{a}{b}\right\rceil, \\
\operatorname{ceilMod}(a,b) &= a-b\operatorname{ceilDiv}(a,b), \\
\operatorname{roundDiv}(a,b) &= \operatorname{round}\left(\frac{a}{b}\right), \\
\operatorname{roundMod}(a,b) &= a-b\operatorname{roundDiv}(a,b).
\end{aligned}
$$

`round`は最も近い整数を返し、ちょうど中間なら0から遠い方を選ぶ。したがって全ての`Div`・`Mod`の組について $a=qb+r$ が成立する。

## Complexity
| Operation | Complexity |
| --- | --- |
| Time | $O(1)$ |
| Space | $O(1)$ |

## Usage
- 除数 `b` は0以外であること
- 各`Mod`関数は対応する`Div`関数の商 $q$ に対する $a-qb$ を返す
- `std::integral`
には符号なし整数も含まれるが、負の剰余を表現する可能性がある場合は符号付き整数を使用する
- 符号付き整数の最小値を `-1` で割る操作は未定義動作となる
cpsc:text:end */

// cpsc:subsnippet:start floorDivNonnegative
template <std::integral T>
[[nodiscard]] constexpr T floorDivNonnegative(T a, T b) noexcept {
  return a / b;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start floorModNonnegative
template <std::integral T>
[[nodiscard]] constexpr T floorModNonnegative(T a, T b) noexcept {
  return a % b;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilDivNonnegative
template <std::integral T>
[[nodiscard]] constexpr T ceilDivNonnegative(T a, T b) noexcept {
  return (a + b - 1) / b;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilModNonnegative
template <std::integral T>
[[nodiscard]] constexpr T ceilModNonnegative(T a, T b) noexcept {
  return a - ((a + b - 1) / b) * b;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start roundDivNonnegative
template <std::integral T>
[[nodiscard]] constexpr T roundDivNonnegative(T a, T b) noexcept {
  return (a + b / 2) / b;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start roundModNonnegative
template <std::integral T>
[[nodiscard]] constexpr T roundModNonnegative(T a, T b) noexcept {
  return a - ((a + b / 2) / b) * b;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Nonnegative Version

`floorDivNonnegative`、`ceilDivNonnegative`、`roundDivNonnegative`と対応する`Mod`関数は、$a \ge 0$、$b>0$に限定した短縮版。

| Operation | Additional requirement |
| --- | --- |
| Floor | なし |
| Ceil | $a+b-1$が`T`で表現可能 |
| Round | $a+\lfloor b/2\rfloor$が`T`で表現可能 |

汎用版より短く、その場で書きやすい一方、負数と加算overflowを扱えない。入力制約から前提を即座に確認できる場合だけ使用する。各`Mod`関数は対応する商$q$に対する$a-qb$を返す。
cpsc:text:end */
