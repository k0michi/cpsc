/* cpsc:meta:start
slug: number-theoretic-transform
title: Number Theoretic Transform
shortTitle: NTT
category: Math
description: ModInt上の数論変換と、任意長の列に対する高速な畳み込みを行う。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

#include "modint.hh"         // cpsc:dependency ModInt
#include "primitive-root.hh" // cpsc:dependency primitiveRoot

// cpsc:subsnippet:start numberTheoreticTransform
template <unsigned long long M, unsigned long long G = PrimitiveRoot<M>>
void numberTheoreticTransform(std::vector<ModInt<M>> &values, bool inverse) {
  using Mint = ModInt<M>;
  std::size_t size = values.size();
  if (size <= 1) {
    return;
  }

  assert(std::has_single_bit(size));
  assert(size <= M - 1 && (M - 1) % size == 0);

  for (std::size_t i = 1, j = 0; i < size; ++i) {
    std::size_t bit = size >> 1;
    while (j & bit) {
      j ^= bit;
      bit >>= 1;
    }
    j ^= bit;
    if (i < j) {
      std::swap(values[i], values[j]);
    }
  }

  for (std::size_t length = 2; length <= size; length <<= 1) {
    Mint root = Mint(G).pow((M - 1) / length);
    if (inverse) {
      root = root.inv();
    }

    for (std::size_t first = 0; first < size; first += length) {
      Mint power = 1;
      for (std::size_t offset = 0; offset < length / 2; ++offset) {
        Mint even = values[first + offset];
        Mint odd = values[first + offset + length / 2] * power;
        values[first + offset] = even + odd;
        values[first + offset + length / 2] = even - odd;
        power *= root;
      }
    }
  }

  if (inverse) {
    Mint inverseSize = Mint(size).inv();
    for (Mint &value : values) {
      value *= inverseSize;
    }
  }
}

template <unsigned long long M, unsigned long long G = PrimitiveRoot<M>>
[[nodiscard]] std::vector<ModInt<M>> convolve(std::vector<ModInt<M>> left,
                                              std::vector<ModInt<M>> right) {
  if (left.empty() || right.empty()) {
    return {};
  }

  std::size_t resultSize = left.size() + right.size() - 1;
  std::size_t transformSize = std::bit_ceil(resultSize);
  assert(transformSize <= M - 1 && (M - 1) % transformSize == 0);

  left.resize(transformSize);
  right.resize(transformSize);
  numberTheoreticTransform<M, G>(left, false);
  numberTheoreticTransform<M, G>(right, false);

  for (std::size_t i = 0; i < transformSize; ++i) {
    left[i] *= right[i];
  }

  numberTheoreticTransform<M, G>(left, true);
  left.resize(resultSize);
  return left;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Requirements
- $M$ は素数、$G$ は法 $M$ の原始根とする
- `numberTheoreticTransform` に渡す列長 $N$ は $2$ の冪で、$N \mid (M-1)$ を満たす
- `convolve` では、結果長以上の最小の $2$ の冪を $N$ としたとき $N \mid (M-1)$ を満たす

前提条件に違反した列長はassertionに失敗する。$M$ と $G$ が素数・原始根であることは実行時には検査しない。

## Complexity
列長を $N$ とすると、数論変換は時間 $O(N\log N)$、追加空間 $O(1)$。長さ $A,B$ の列の畳み込みは $N=2^{\lceil\log_2(A+B-1)\rceil}$ として時間 $O(N\log N)$、空間 $O(N)$。

## Usage
- `using Mint = ModInt<998244353>;` に対して `convolve(a, b)` で畳み込みを得る。法は引数から推論され、原始根はcompile-timeに決まる
- `numberTheoreticTransform(values, false)` が順変換、`true` が逆変換を行う
- 原始根を明示する場合は `convolve<998244353, 3>(a, b)` のように指定する
- 畳み込みの入力の一方でも空なら空列を返す
cpsc:text:end */
