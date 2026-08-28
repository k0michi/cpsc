/* cpsc:meta:start
slug: divisors
title: Divisors
shortTitle: Divisors
category: Math
description: 正の整数が持つ正の約数を昇順で列挙する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start divisors
template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::vector<T> divisors(T n) {
  assert(n >= T{1});

  std::vector<T> lower;
  std::vector<T> upper;

  for (T divisor = T{1}; divisor <= n / divisor; ++divisor) {
    if (n % divisor != 0) {
      continue;
    }

    lower.push_back(divisor);
    T quotient = n / divisor;
    if (quotient != divisor) {
      upper.push_back(quotient);
    }
  }

  lower.insert(lower.end(), upper.rbegin(), upper.rend());
  return lower;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

$n \ge 1$ に対し、$n$ を割り切る正の整数を昇順かつ重複なしで返す。

$$
\mathrm{divisors}(n)=\{d\in\mathbb{Z}_{>0}\mid d\mid n\}
$$

`n == 1` では `{1}` を返す。

## Requirements

入力は $n \ge 1$ とする。違反は `assert` で検出する。

## Complexity

約数の個数を $\tau(n)$ とすると、時間計算量は $O(\sqrt n)$、返り値を含む空間計算量は $O(\tau(n))$。

## Usage

```cpp
auto result = divisors(12);
// {1, 2, 3, 4, 6, 12}
```

## Notes

- $\sqrt n$ 以下の約数と対応する商を別々に蓄積し、sortせず昇順に並べる
- loop条件に `divisor <= n / divisor` を使い、`divisor * divisor` のoverflowを避ける
cpsc:text:end */
