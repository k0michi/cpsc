/* cpsc:meta:start
slug: prime-factorization
title: Prime Factorization
shortTitle: Factorization
category: Math
description: 試し割りにより、正の整数を素因数と指数の列へ分解する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start primeFactorize
template <std::integral T>
  requires(!std::same_as<std::remove_cv_t<T>, bool>)
[[nodiscard]] constexpr std::vector<std::pair<T, int>> primeFactorize(T n) {
  assert(n >= T{1});

  std::vector<std::pair<T, int>> factors;

  for (T divisor = T{2}; divisor <= n / divisor; ++divisor) {
    if (n % divisor != 0) {
      continue;
    }

    int exponent = 0;
    do {
      n /= divisor;
      ++exponent;
    } while (n % divisor == 0);

    factors.emplace_back(divisor, exponent);
  }

  if (n > T{1}) {
    factors.emplace_back(n, 1);
  }
  return factors;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

$n \ge 1$ に対し、次を満たす素因数分解を返す。

$$
n=\prod_{(p,e)\in\mathrm{primeFactorize}(n)}p^e
$$

返り値の型は `std::vector<std::pair<T, int>>` とし、各要素は `{prime, exponent}` を表す。素因数は昇順で重複しない。`n == 1` では空のvectorを返す。

この型を素因数分解の共通interfaceとし、約数列挙、約数個数、Eulerのtotient関数などでも同じ表現を使用する。

## Requirements

入力は $n \ge 1$ とする。違反は `assert` で検出する。

## Complexity

時間計算量は最悪 $O(\sqrt n)$、返り値を除く追加空間は $O(1)$。

## Usage

```cpp
auto factors = primeFactorize(360LL);
// {{2, 3}, {3, 2}, {5, 1}}
```

## Notes

- loop条件は `divisor * divisor <= n` ではなく `divisor <= n / divisor` とし、乗算のoverflowを避ける
- 大きな64-bit整数では Pollard's rho法などを使う
cpsc:text:end */
