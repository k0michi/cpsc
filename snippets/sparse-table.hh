/* cpsc:meta:start
slug: sparse-table
title: Sparse Table
shortTitle: Sparse Table
category: Data Structure
description: 冪等半群の静的な区間積を、前計算後に定数時間で求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start SparseTable
template <typename T, typename O> class SparseTable {
public:
  using value_type = T;
  using size_type = std::size_t;

  SparseTable() : SparseTable(0) {}

  explicit SparseTable(size_type length, const value_type &value = value_type{},
                       O op = O{})
      : SparseTable(std::vector<value_type>(length, value), std::move(op)) {}

  explicit SparseTable(const std::vector<value_type> &values, O op = O{})
      : length_(values.size()), op_(std::move(op)) {
    if (empty()) {
      return;
    }

    size_type levelCount = std::bit_width(length_);
    table_.reserve(levelCount);
    table_.push_back(values);

    for (size_type level = 1; level < levelCount; ++level) {
      size_type halfLength = size_type{1} << (level - 1);
      size_type blockLength = size_type{1} << level;
      size_type blockCount = length_ - blockLength + 1;
      std::vector<value_type> products;
      products.reserve(blockCount);
      for (size_type left = 0; left < blockCount; ++left) {
        products.push_back(
            op_(table_[level - 1][left], table_[level - 1][left + halfLength]));
      }
      table_.push_back(std::move(products));
    }
  }

  template <std::ranges::input_range R>
    requires std::ranges::common_range<R> &&
             std::convertible_to<std::ranges::range_reference_t<R>,
                                 value_type> &&
             (!std::same_as<std::remove_cvref_t<R>, std::vector<value_type>>)
  explicit SparseTable(R &&range, O op = O{})
      : SparseTable(std::vector<value_type>(std::ranges::begin(range),
                                            std::ranges::end(range)),
                    std::move(op)) {}

  [[nodiscard]] value_type product(size_type left, size_type right) const {
    assert(left < right && right <= length_);
    size_type length = right - left;
    size_type level = std::bit_width(length) - 1;
    size_type blockLength = size_type{1} << level;
    return op_(table_[level][left], table_[level][right - blockLength]);
  }

  [[nodiscard]] value_type allProduct() const {
    assert(!empty());
    return product(0, length_);
  }

  [[nodiscard]] const value_type &get(size_type index) const {
    assert(index < length_);
    return table_[0][index];
  }

  [[nodiscard]] const value_type &operator[](size_type index) const {
    return get(index);
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, size_type right,
                                         P predicate) const
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    if (left == right || !predicate(get(left))) {
      return left;
    }

    size_type satisfied = left + 1;
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

  [[nodiscard]] size_type size() const noexcept {
    return length_;
  }

  [[nodiscard]] bool empty() const noexcept {
    return length_ == 0;
  }

private:
  size_type length_;
  std::vector<std::vector<value_type>> table_;
  [[no_unique_address]] O op_;
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Requirements

値の集合を$S$とする。`op`は$S$上の閉じた二項演算であり、結合律と冪等律を満たす必要がある。

$$
\begin{aligned}
\operatorname{op}&:S\times S\to S, \\
\operatorname{op}(\operatorname{op}(a,b),c)
&=\operatorname{op}(a,\operatorname{op}(b,c)), \\
\operatorname{op}(a,a)&=a
\qquad(a,b,c\in S).
\end{aligned}
$$

つまり$(S,\operatorname{op})$は冪等半群となる。可換律と単位元は不要。`product`は区間を覆う2個の同じ長さのblockを重ねて計算し、重複部分を冪等律によって無視する。

## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N\log N)$ |
| `product`, `get`, `allProduct` | $O(1)$ |
| `partitionPoint` | $O(\log N)$ |
| Space | $O(N\log N)$ |

## Usage
- 最小値、最大値、GCD、bitwise AND、bitwise ORなどに使える
- `product(left, right)`は空でない半開区間$[left,right)$の積を返す
- `allProduct()`は空のtableでは呼べない
- `partitionPoint`は`predicate(product(left,r))`が$r$についてtrueからfalseへ単調に変化するとき、条件を満たす最大の$r$を返す
- `predicate(get(left))`がfalseの場合と空の探索区間では`left`を返す
- 構築後の列は変更できない。更新が必要ならSegment Treeを使う
cpsc:text:end */
