/* cpsc:meta:start
slug: primality-test
title: Primality Test
shortTitle: isPrime
category: Math
description: 試し割りにより、整数が素数か判定する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start isPrime
template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr bool isPrime(T n) noexcept {
  if (n < T{2}) {
    return false;
  }
  if (n == T{2} || n == T{3}) {
    return true;
  }
  if (n % T{2} == 0 || n % T{3} == 0) {
    return false;
  }

  for (T divisor = T{5}; divisor <= n / divisor; divisor += T{6}) {
    if (n % divisor == 0 || n % (divisor + T{2}) == 0) {
      return false;
    }
  }
  return true;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

$n \ge 2$ かつ、$1$ と $n$ 以外に正の約数を持たないとき `true` を返す。負数、`0`、`1` は `false`。

## Complexity

時間計算量は $O(\sqrt n)$、空間計算量は $O(1)$。

## Usage

```cpp
if (isPrime(n)) {
  // n is prime
}
```

## Notes

- `2` と `3` を先に処理し、それ以降は $6k\pm1$ の候補だけを試し割りする
- loop条件は `divisor * divisor <= n` ではなく `divisor <= n / divisor` とし、乗算のoverflowを避ける
- 多数の値や大きな64-bit整数を判定する用途では、Miller–Rabin法などを使う
cpsc:text:end */
