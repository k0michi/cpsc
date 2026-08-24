/* cpsc:meta:start
slug: chinese-remainder-theorem
title: Chinese Remainder Theorem
shortTitle: CRT
category: Math
description: 複数の合同式を、同値な1つの合同式へ統合する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

/* cpsc:text:start
## Problem
次の2つの合同式を同時に満たす整数 $x$ を求める。

$$
\begin{aligned}
x &\equiv r_1 \pmod{m_1} \\
x &\equiv r_2 \pmod{m_2}
\end{aligned}
$$

解が存在する条件は次の通り。

$$
r_1 \equiv r_2 \pmod{\gcd(m_1,m_2)}
$$

解が存在する場合、戻り値 $(r,m)$ は全ての解を次の1つの合同式で表す。

$$
x \equiv r \pmod m,
\qquad
m=\operatorname{lcm}(m_1,m_2)
$$

配列版は合同式 $x \equiv r_i \pmod{m_i}$ を順に統合し、全てを同時に満たす解を求める。

## Algorithm
拡張Euclid法で2つの法の最大公約数とBézout係数を求め、両方の合同式を満たす係数を計算する。法が互いに素でない場合にも対応し、剰余の差が最大公約数で割り切れなければ解なしと判定する。

配列版は初期値 $x \equiv 0 \pmod 1$ から合同式を順番に統合する。

## Complexity
| Function | Complexity |
| --- | --- |
| Two congruences | $O(\log \min(m_1,m_2))$ |
| $N$ congruences | $O(N \log M)$ |

## Usage
- 各法は正の整数とする
- 戻り値 `(r, m)` は $0 \le r < m$ を満たし、解全体を $x \equiv r \pmod m$ として表す
- 解が存在しない場合は `std::nullopt` を返す
- 統合後の法の最小公倍数が `long long` に収まること
- 係数計算にGCC・Clangの `__int128_t` 拡張を使用する
cpsc:text:end */

// cpsc:subsnippet:start Chinese Remainder Theorem
constexpr std::tuple<long long, long long, long long> extendedGCD(long long a,
                                                                 long long b) {
  if (b == 0) {
    return {a, 1, 0};
  }
  auto [gcd, y, x] = extendedGCD(b, a % b);
  y -= a / b * x;
  return {gcd, x, y};
}

constexpr std::optional<std::pair<long long, long long>> crt(long long r1,
                                                             long long m1,
                                                             long long r2,
                                                             long long m2) {
  long long rem1 = (r1 % m1 + m1) % m1;
  long long rem2 = (r2 % m2 + m2) % m2;

  auto [g, p, q] = extendedGCD(m1, m2);

  if ((rem2 - rem1) % g != 0) {
    return std::nullopt;
  }

  long long u = m2 / g;
  long long diff = (rem2 - rem1) / g;
  long long factor = static_cast<long long>(
      (__int128_t(diff % u) * (p % u) % u + u) % u);

  long long mergedMod = m1 * u;
  long long mergedRem = rem1 + m1 * factor;

  return std::make_pair(mergedRem, mergedMod);
}

constexpr std::optional<std::pair<long long, long long>> crt(
    const std::vector<std::pair<long long, long long>>& congruences) {
  std::pair<long long, long long> current{0, 1};

  for (auto&& [rem, mod] : congruences) {
    auto result = crt(current.first, current.second, rem, mod);
    if (!result) {
      return std::nullopt;
    }
    current = *result;
  }

  return current;
}
// cpsc:subsnippet:end
