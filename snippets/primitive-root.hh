/* cpsc:meta:start
slug: primitive-root
title: Primitive Root
shortTitle: Primitive Root
category: Math
description: 素数を法とする最小の原始根をconstexprに求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start primitiveRoot
constexpr unsigned long long modularPower(unsigned long long base,
                                          unsigned long long exponent,
                                          unsigned long long modulus) {
  unsigned long long result = 1;
  base %= modulus;
  while (exponent > 0) {
    if (exponent & 1) {
      result = static_cast<unsigned long long>(
          static_cast<__uint128_t>(result) * base % modulus);
    }
    base = static_cast<unsigned long long>(static_cast<__uint128_t>(base) *
                                           base % modulus);
    exponent >>= 1;
  }
  return result;
}

constexpr unsigned long long primitiveRoot(unsigned long long modulus) {
  assert(modulus >= 2);
  if (modulus == 2) {
    return 1;
  }

  unsigned long long primeFactors[32]{};
  std::size_t factorCount = 0;
  unsigned long long remaining = modulus - 1;
  for (unsigned long long factor = 2; factor * factor <= remaining; ++factor) {
    if (remaining % factor != 0) {
      continue;
    }
    primeFactors[factorCount++] = factor;
    while (remaining % factor == 0) {
      remaining /= factor;
    }
  }
  if (remaining > 1) {
    primeFactors[factorCount++] = remaining;
  }

  for (unsigned long long candidate = 2; candidate < modulus; ++candidate) {
    bool isPrimitive = true;
    for (std::size_t i = 0; i < factorCount; ++i) {
      if (modularPower(candidate, (modulus - 1) / primeFactors[i], modulus) ==
          1) {
        isPrimitive = false;
        break;
      }
    }
    if (isPrimitive) {
      return candidate;
    }
  }

  assert(false);
  return 0;
}

template <unsigned long long M>
inline constexpr unsigned long long PrimitiveRoot = primitiveRoot(M);
// cpsc:subsnippet:end

/* cpsc:text:start
## Requirements
- 法 $M$ は素数かつ $2 \le M \le \mathrm{LLONG\_MAX}$ とする
- 素数であることは実行時には検査しない

## Complexity
$M-1$ の試し割りに時間 $O(\sqrt M)$、異なる素因数の保存に空間 $O(\log M)$ を使う。その後、条件を満たす最小の原始根まで順に探索する。

## Usage
- `primitiveRoot(17)` は最小の原始根 `3` を返す
- `PrimitiveRoot<998244353>` はcompile-time定数 `3` になる
- 乗算の中間値にGCC・Clangの `__uint128_t` 拡張を使用する
cpsc:text:end */
