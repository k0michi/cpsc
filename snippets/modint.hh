/* cpsc:meta:start
slug: modint
title: Modular Integer
shortTitle: ModInt
category: Math
description: 値を法 M で自動的に正規化し、通常の整数と同じ感覚で剰余付きの四則演算を行う。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| `extendedGCD(a, b)` | $O(\log \min(a, b))$ |
| Addition / Subtraction / Multiplication | $O(1)$ |
| Division | $O(\log M)$ |
| `pow(a, n)` | $O(\log n)$ |

## Usage
- `extendedGCD(a, b)` は $(\gcd(a,b), x, y)$ を返し、$ax + by = \gcd(a,b)$ を満たす
- `extendedGCD` には非負整数を渡す
- `using Mint = ModInt<1'000'000'007>;` のように法をコンパイル時に指定する
- 構築時と入力時に負数を含む整数を $[0, M)$ へ正規化する
- $0 < M \leq \mathrm{LLONG\_MAX}$ を満たす法を使用する
- 除算は除数と $M$ が互いに素な場合に限る。逆元が存在しない場合はassertionに失敗する
- 乗算の中間値にGCC・Clangの `__uint128_t` 拡張を使用する
cpsc:text:end */

// cpsc:subsnippet:start ModInt
// cpsc:subsnippet:description 拡張Euclid法を使い、値の正規化、四則演算、累乗、逆元を提供する。
constexpr std::tuple<long long, long long, long long> extendedGCD(long long a,
                                                                  long long b) {
  if (b == 0) {
    return {a, 1, 0};
  }
  auto [gcd, y, x] = extendedGCD(b, a % b);
  y -= a / b * x;
  return {gcd, x, y};
}

template <unsigned long long M> struct ModInt {
  static constexpr unsigned long long Modulus = M;

  constexpr ModInt() : mValue(0) {}

  template <typename T, std::enable_if_t<std::is_signed_v<T>> * = nullptr>
  constexpr ModInt(T value) {
    long long normalized =
        static_cast<long long>(value) % static_cast<long long>(Modulus);
    if (normalized < 0) {
      normalized += Modulus;
    }
    mValue = static_cast<unsigned long long>(normalized);
  }

  template <typename T, std::enable_if_t<std::is_unsigned_v<T>> * = nullptr>
  constexpr ModInt(T value)
      : mValue(static_cast<unsigned long long>(value) % Modulus) {}

  constexpr unsigned long long val() const {
    return mValue;
  }
  constexpr long long toLLong() const {
    return static_cast<long long>(mValue);
  }
  constexpr unsigned long long toULLong() const {
    return mValue;
  }

  constexpr ModInt &operator+=(ModInt r) {
    mValue += r.mValue;

    if (mValue >= Modulus) {
      mValue -= Modulus;
    }

    return *this;
  }

  constexpr ModInt &operator-=(ModInt r) {
    if (mValue < r.mValue) {
      mValue += Modulus;
    }
    mValue -= r.mValue;
    return *this;
  }

  constexpr ModInt<M> &operator*=(const ModInt<M> &r) {
    mValue = static_cast<unsigned long long>(static_cast<__uint128_t>(mValue) *
                                             r.mValue % Modulus);
    return *this;
  }

  constexpr ModInt &operator/=(ModInt r) {
    return *this *= r.inv();
  }

  constexpr ModInt operator+() const {
    return *this;
  }
  constexpr ModInt operator-() const {
    return ModInt{} - *this;
  }

  constexpr ModInt &operator++() {
    return *this += 1;
  }
  constexpr ModInt &operator--() {
    return *this -= 1;
  }

  constexpr ModInt<M> operator++(int) {
    ModInt<M> result = *this;
    ++*this;
    return result;
  }

  constexpr ModInt<M> operator--(int) {
    ModInt<M> result = *this;
    --*this;
    return result;
  }

  constexpr bool operator==(ModInt r) const {
    return mValue == r.mValue;
  }
  constexpr bool operator!=(ModInt r) const {
    return mValue != r.mValue;
  }

  constexpr ModInt pow(unsigned long long exp) const {
    ModInt res = 1, base = *this;
    while (exp > 0) {
      if (exp & 1) {
        res *= base;
      }
      base *= base;
      exp >>= 1;
    }
    return res;
  }

  constexpr ModInt inv() const {
    auto [g, x, y] = extendedGCD(static_cast<long long>(mValue),
                                 static_cast<long long>(Modulus));
    assert(g == 1);
    return ModInt(x);
  }

  static constexpr ModInt pow(ModInt a, unsigned long long exp) {
    return a.pow(exp);
  }
  static constexpr ModInt invert(ModInt a) {
    return a.inv();
  }

  friend constexpr ModInt operator+(ModInt l, ModInt r) {
    return l += r;
  }
  friend constexpr ModInt operator-(ModInt l, ModInt r) {
    return l -= r;
  }
  friend constexpr ModInt operator*(ModInt l, ModInt r) {
    return l *= r;
  }
  friend constexpr ModInt operator/(ModInt l, ModInt r) {
    return l /= r;
  }

  friend std::ostream &operator<<(std::ostream &s, ModInt a) {
    return s << a.mValue;
  }

  friend std::istream &operator>>(std::istream &s, ModInt &a) {
    long long value;
    if (s >> value) {
      a = ModInt(value);
    }
    return s;
  }

private:
  unsigned long long mValue;
};
// cpsc:subsnippet:end
