/* cpsc:meta:start
slug: linear-sieve
title: Linear Sieve
shortTitle: Linear Sieve
category: Math
description: 最小素因数を線形時間で前計算し、素数判定・素因数分解・乗法的関数の列挙を行う。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start LinearSieve
template <std::integral T = int>
  requires(!std::same_as<std::remove_cv_t<T>, bool>) class LinearSieve {
public:
  using value_type = T;
  using size_type = std::size_t;

  explicit LinearSieve(value_type maxN)
      : maxN_(checkedMaxN(maxN)), minFactors_(checkedSize(maxN_), T{}) {
    minFactors_[1] = T{1};

    for (value_type value = T{2}; value <= maxN_; ++value) {
      if (minFactors_[value] == T{}) {
        minFactors_[value] = value;
        primes_.push_back(value);
      }
      for (value_type prime : primes_) {
        if (prime > minFactors_[value] || value > maxN_ / prime) {
          break;
        }
        minFactors_[value * prime] = prime;
      }
    }
  }

  [[nodiscard]] bool isPrime(value_type value) const {
    assert(T{} <= value && value <= maxN_);
    return value >= T{2} && minFactors_[value] == value;
  }

  [[nodiscard]] value_type minFactor(value_type value) const {
    assert(T{1} <= value && value <= maxN_);
    return minFactors_[value];
  }

  [[nodiscard]] const std::vector<value_type> &primes() const noexcept {
    return primes_;
  }

  [[nodiscard]] value_type maxN() const noexcept {
    return maxN_;
  }

  [[nodiscard]] std::vector<std::pair<value_type, int>>
  primeFactorize(value_type value) const {
    assert(T{1} <= value && value <= maxN_);

    std::vector<std::pair<value_type, int>> factors;
    while (value > T{1}) {
      const value_type prime = minFactors_[value];
      int exponent = 0;
      do {
        value /= prime;
        ++exponent;
      } while (value % prime == 0);
      factors.emplace_back(prime, exponent);
    }
    return factors;
  }

  template <typename V, typename O, typename F>
    requires std::invocable<O &, const V &, const V &> &&
             std::convertible_to<
                 std::invoke_result_t<O &, const V &, const V &>, V> &&
             std::invocable<F &, value_type, int, value_type> &&
             std::convertible_to<
                 std::invoke_result_t<F &, value_type, int, value_type>, V>
  [[nodiscard]] std::vector<V> generate(V unit, O operation,
                                        F primePowerValue) const {
    std::vector<V> values(minFactors_.size());
    std::vector<value_type> leastPrimePowers(minFactors_.size());
    std::vector<int> exponents(minFactors_.size());
    values[1] = std::move(unit);

    for (value_type value = T{2}; value <= maxN_; ++value) {
      const value_type prime = minFactors_[value];
      const value_type quotient = value / prime;
      if (quotient == T{1} || minFactors_[quotient] != prime) {
        leastPrimePowers[value] = prime;
        exponents[value] = 1;
      } else {
        leastPrimePowers[value] = leastPrimePowers[quotient] * prime;
        exponents[value] = exponents[quotient] + 1;
      }

      const value_type primePower = leastPrimePowers[value];
      if (value == primePower) {
        values[value] =
            std::invoke(primePowerValue, prime, exponents[value], primePower);
      } else {
        values[value] = std::invoke(operation, values[primePower],
                                    values[value / primePower]);
      }
    }
    return values;
  }

private:
  [[nodiscard]] static value_type checkedMaxN(value_type maxN) {
    assert(maxN >= T{1});
    return maxN;
  }

  [[nodiscard]] static size_type checkedSize(value_type maxN) {
    assert(std::in_range<size_type>(maxN));
    return static_cast<size_type>(maxN) + 1;
  }

  value_type maxN_;
  std::vector<value_type> minFactors_;
  std::vector<value_type> primes_;
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

`LinearSieve<T>(maxN)`は$1$から`maxN`までの最小素因数をEulerの線形篩で前計算する。`T`は`bool`以外の整数型とし、省略時はconstructor argumentから推論される。

- `isPrime(value)`は素数か判定する
- `minFactor(value)`は最小素因数を返す。`minFactor(1) == 1`
- `primes()`は`maxN`以下の素数を昇順で返す
- `primeFactorize(value)`は`{prime, exponent}`の列を素数の昇順で返す。$1$では空のvectorを返す

`generate<V>(unit, operation, primePowerValue)`は、素数冪ごとの値を二項演算で合成し、$F(0),F(1),\ldots,F(\mathrm{maxN})$をまとめて返す。`primePowerValue`には素数$p$、指数$k$、素数冪$p^k$が渡される。互いに異なる素因数$p_i$について、素数の昇順で次を計算する。

$$
F\left(\prod_i p_i^{e_i}\right)
=f(p_1^{e_1})\mathbin{\mathrm{operation}}\cdots
\mathbin{\mathrm{operation}}f(p_m^{e_m})
$$

`values[0]`はvalue-initializeされ、`values[1] == unit`となる。`operation`は乗算に限らず、加算、bit演算、`max`、関数合成などを使用できる。非可換な演算でも素因数の昇順に適用するため、結果の順序は一意に定まる。

## Requirements

- 構築時に`maxN >= 1`が必要
- queryの`value`は`isPrime`では$0\le value\le\mathrm{maxN}$、それ以外では$1\le value\le\mathrm{maxN}$とする
- `operation`は`V`上の結合的な二項演算、`unit`はその単位元とする
- `primePowerValue(p, k, primePower)`の返り値と`operation`の結果は`V`へ変換できるものとする

範囲違反は`assert`で検出する。

## Complexity

| Operation | Time | Space |
| --- | --- | --- |
| Construction | $O(N)$ | $O(N)$ |
| `isPrime`, `minFactor`, `primes`, `maxN` | $O(1)$ | $O(1)$ |
| `primeFactorize(value)` | $O(\log value)$ | $O(\log value)$ |
| `generate` | $O(N)$ | $O(N)$ |

返り値の領域をspaceに含む。

## Usage

```cpp
LinearSieve sieve(1'000'000);
auto factors = sieve.primeFactorize(360); // {{2, 3}, {3, 2}, {5, 1}}

auto phi = sieve.generate<int>(
    1, std::multiplies{}, [](auto prime, int, auto primePower) {
      return primePower / prime * (prime - 1);
    });
// phi[360] == 96

auto omega = sieve.generate<int>(
    0, std::plus{}, [](auto, int, auto) { return 1; });
// omega[360] == 3
```
cpsc:text:end */
