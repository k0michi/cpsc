/* cpsc:meta:start
slug: extended-gcd
title: Extended Euclidean Algorithm
shortTitle: Extended GCD
category: Math
description: 拡張Euclid法により最大公約数とBezout係数を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start extendedGCD
[[nodiscard]] constexpr std::tuple<long long, long long, long long>
extendedGCD(long long a, long long b) {
  if (b == 0) {
    return {a, 1, 0};
  }
  auto [gcd, y, x] = extendedGCD(b, a % b);
  y -= a / b * x;
  return {gcd, x, y};
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

`extendedGCD(a, b)`は次を満たす`{gcd, x, y}`を返す。

$$
ax+by=\mathrm{gcd}
$$

`gcd`の符号は再帰の終端に到達した非零値の符号に従う。特に$a,b\ge0$では`gcd`も非負となる。`extendedGCD(0, 0)`は`{0, 1, 0}`を返す。

## Complexity

$a,b$の絶対値の最大を$M$とすると、時間計算量とcall stackは$O(\log M)$。

## Usage

```cpp
auto [gcd, x, y] = extendedGCD(30, 18);
// gcd == 6, 30 * x + 18 * y == gcd
```
cpsc:text:end */
