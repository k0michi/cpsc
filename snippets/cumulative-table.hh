/* cpsc:meta:start
slug: cumulative-table
title: Cumulative Table
shortTitle: Cumulative Table
category: Data Structure
description: 群の列から累積積のtableを構築し、静的な区間積を定数時間で計算する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start CumulativeTable
template <typename T, typename O = std::plus<T>, typename I = std::negate<T>>
class CumulativeTable {
public:
  using value_type = T;
  using size_type = std::size_t;

  CumulativeTable() : CumulativeTable(0) {}

  explicit CumulativeTable(size_type length, value_type unit = value_type{},
                           O op = O{}, I inverse = I{})
      : CumulativeTable(std::vector<value_type>(length, unit), unit,
                        std::move(op), std::move(inverse)) {}

  explicit CumulativeTable(const std::vector<value_type> &values,
                           value_type unit = value_type{}, O op = O{},
                           I inverse = I{})
      : length_(values.size()), cumulativeProducts_(length_ + 1, unit),
        op_(std::move(op)), inverse_(std::move(inverse)), unit_(unit) {
    for (size_type i = 0; i < length_; ++i) {
      cumulativeProducts_[i + 1] = op_(cumulativeProducts_[i], values[i]);
    }
  }

  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_reference_t<R>, value_type>
  explicit CumulativeTable(R &&range, value_type unit = value_type{},
                           O op = O{}, I inverse = I{})
      : length_(0), cumulativeProducts_{unit}, op_(std::move(op)),
        inverse_(std::move(inverse)), unit_(unit) {
    for (auto &&value : range) {
      cumulativeProducts_.push_back(op_(cumulativeProducts_.back(), value));
      ++length_;
    }
  }

  [[nodiscard]] const value_type &allProduct() const {
    return cumulativeProducts_.back();
  }

  [[nodiscard]] value_type product(size_type left, size_type right) const {
    assert(left <= right && right <= length_);
    if (left == right) {
      return unit_;
    }
    if (left == 0) {
      return cumulativeProducts_[right];
    }
    return op_(inverse_(cumulativeProducts_[left]), cumulativeProducts_[right]);
  }

  [[nodiscard]] value_type get(size_type index) const {
    assert(index < length_);
    return product(index, index + 1);
  }

  [[nodiscard]] value_type operator[](size_type index) const {
    return get(index);
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, size_type right,
                                         P predicate) const
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));

    size_type satisfied = left;
    size_type unsatisfied = right + 1;
    while (unsatisfied - satisfied > 1) {
      size_type middle = satisfied + (unsatisfied - satisfied) / 2;
      if (predicate(product(left, middle))) {
        satisfied = middle;
      } else {
        unsatisfied = middle;
      }
    }
    return satisfied;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, P predicate) const
    requires std::predicate<P, value_type> {
    return partitionPoint(left, length_, std::move(predicate));
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type left, size_type right,
                                                P predicate) const
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));
    if (left == right || predicate(product(left, right))) {
      return left;
    }

    size_type unsatisfied = left;
    size_type satisfied = right;
    while (satisfied - unsatisfied > 1) {
      size_type middle = unsatisfied + (satisfied - unsatisfied) / 2;
      if (predicate(product(middle, right))) {
        satisfied = middle;
      } else {
        unsatisfied = middle;
      }
    }
    return satisfied;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type right,
                                                P predicate) const
    requires std::predicate<P, value_type> {
    return partitionPointReverse(0, right, std::move(predicate));
  }

  [[nodiscard]] size_type size() const noexcept {
    return length_;
  }

  [[nodiscard]] bool empty() const noexcept {
    return length_ == 0;
  }

private:
  size_type length_;
  std::vector<value_type> cumulativeProducts_;
  [[no_unique_address]] O op_;
  [[no_unique_address]] I inverse_;
  value_type unit_;
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Requirements

値の集合を$G$とする。`op`、`unit`、`inverse`は群$(G,\operatorname{op},\mathrm{unit},\operatorname{inverse})$をなす必要がある。

`op`は結合律を満たし、`unit`は両側単位元となる。

$$
\begin{aligned}
\operatorname{op}(\operatorname{op}(a,b),c)
&=\operatorname{op}(a,\operatorname{op}(b,c)), \\
\operatorname{op}(\mathrm{unit},a)
&=\operatorname{op}(a,\mathrm{unit})=a.
\end{aligned}
$$

`inverse(a)`は`a`の両側逆元を返す。

$$
\operatorname{op}(\operatorname{inverse}(a),a)
=\operatorname{op}(a,\operatorname{inverse}(a))
=\mathrm{unit}.
$$

可換則は不要。prefix積$P_i=a_0\operatorname{op}\cdots\operatorname{op}a_{i-1}$に対し、区間積は次の順序で求める。

$$
\operatorname{product}(l,r)
=\operatorname{op}(\operatorname{inverse}(P_l),P_r).
$$

## Terminology

加法`std::plus`に限定したものは、競技プログラミングでは一般に累積和またはprefix sumと呼ばれる。`CumulativeTable`は同じ構成を任意の群へ一般化したもの。

内部では$P_0$から$P_N$までの$N+1$要素を保持する。先頭から$P_{N-1}$までは`std::exclusive_scan`に対応し、この実装では全体積$P_N$も末尾に保持する。

## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N)$ |
| `product`, `get`, `allProduct` | $O(1)$ |
| `partitionPoint`, `partitionPointReverse` | $O(\log N)$ |
| Space | $O(N)$ |

## Usage
- 既定値は加法群で、`op = std::plus<T>`、`inverse = std::negate<T>`、`unit = T{}`となる
- `product(left, right)`は半開区間$[left,right)$の積を返す
- 構築後の列は変更できない。更新が必要ならSegment Treeを使う
- `partitionPoint`では`predicate(product(left,r))`が$r$についてtrueからfalseへ単調に変化することが必要
- `partitionPointReverse`では`predicate(product(l,right))`が$l$についてfalseからtrueへ単調に変化することが必要
- 両方のpredicateは`unit`に対してtrueを返す必要がある
cpsc:text:end */
