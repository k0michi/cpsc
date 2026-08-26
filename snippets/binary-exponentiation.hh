/* cpsc:meta:start
slug: binary-exponentiation
title: Binary Exponentiation
shortTitle: Binary Pow
category: Math
description: 整数指数の累乗を、指数のビットに沿って高速に計算する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start binaryPow
template <typename T, std::integral E>
[[nodiscard]] constexpr T binaryPow(T base, E exp) {
  T result = 1;
  while (exp > 0) {
    if (exp & 1) {
      result *= base;
    }
    if (exp > 1) {
      base *= base;
    }
    exp >>= 1;
  }
  return result;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Algorithm
Binary Exponentiation。指数を2進数として下位ビットから調べ、ビットが1なら現在の `base` を結果へ掛ける。各反復で `base` を二乗するため、$base^{2^i}$ を順に利用できる。

例えば $13=(1101)_2$ なので、$a^{13}=a^8a^4a$ として計算する。

Binary Exponentiationは整数に限らず、単位元と結合則を満たす二項演算を持つ構造、つまりモノイド上の累乗に適用できる。結合則によって同じ要素の反復適用を二乗へまとめられ、可換則は必要ない。この実装では二項演算を `operator*=`、単位元を `1` として表しているため、整数、`ModInt`、行列などに使える。

## Complexity
| Operation | Complexity |
| --- | --- |
| Time | $O(\log exp)$ |
| Space | $O(1)$ |

## Usage
- `exp` は0以上とする。負の符号付き整数を渡した場合は `1` を返す
- `T` は `1` から初期化でき、結合的な乗算と `operator*=` を持つ型とする
- 整数型では計算途中のオーバーフローに注意する
cpsc:text:end */
