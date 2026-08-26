/* cpsc:meta:start
slug: integer-logarithm
title: Integer Logarithm
shortTitle: Integer Log
category: Math
description: 浮動小数点数を使わず、整数の対数を切り上げまたは切り捨てて求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start floorLog2
template <std::integral T> [[nodiscard]] constexpr int floorLog2(T n) {
  using U = std::make_unsigned_t<T>;
  return std::bit_width(static_cast<U>(n)) - 1;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilLog2
template <std::integral T> [[nodiscard]] constexpr int ceilLog2(T n) {
  if (n <= 1) {
    return 0;
  }
  using U = std::make_unsigned_t<T>;
  return std::bit_width(static_cast<U>(n - 1));
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start floorLog
template <std::integral T> [[nodiscard]] constexpr int floorLog(T base, T n) {
  int count = 0;
  while (n >= base) {
    n /= base;
    ++count;
  }
  return count;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilLog
template <std::integral T> [[nodiscard]] constexpr int ceilLog(T base, T n) {
  int count = 0;
  while (n > 1) {
    n = 1 + (n - 1) / base;
    ++count;
  }
  return count;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| Function | Time | Space |
| --- | --- | --- |
| `floorLog2`, `ceilLog2` | $O(1)$ | $O(1)$ |
| `floorLog`, `ceilLog` | $O(\log_b n)$ | $O(1)$ |

## Usage
- `floorLog2(n)` は $n \ge 1$ とする
- `ceilLog2(n)` は $n \le 1$ のとき `0` を返す
- `floorLog(base, n)` と `ceilLog(base, n)` は $base \ge 2, n \ge 1$ とする
- 浮動小数点数の丸め誤差を避け、整数型の最大値まで扱える
cpsc:text:end */
