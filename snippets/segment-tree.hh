/* cpsc:meta:start
slug: segment-tree
title: Segment Tree
shortTitle: Segment Tree
category: Data Structure
description: モノイドの列を保持し、1点更新と区間積を対数時間で処理する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Requirements
値の集合を $S$ とする。この実装が正しく動作するには、`op` と `unit` がモノイド $(S,\operatorname{op},\mathrm{unit})$ をなす必要がある。

`op` は $S$ 上で閉じた二項演算、`unit` は $S$ の要素とする。

$$
\operatorname{op}:S\times S\to S,
\qquad
\mathrm{unit}\in S
$$

`op` は結合律を満たす。

$$
\operatorname{op}(\operatorname{op}(a,b),c)
=\operatorname{op}(a,\operatorname{op}(b,c))
\qquad (a,b,c\in S)
$$

`unit` は `op` の単位元となる。

$$
\operatorname{op}(\mathrm{unit},a)
=\operatorname{op}(a,\mathrm{unit})
=a
\qquad (a\in S)
$$

これらの条件によって、区間の分割方法によらず同じ積を得られ、空区間を `unit` で表せる。

## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N)$ |
| `set` / `product` / `partitionPoint` / `partitionPointReverse` | $O(\log N)$ |
| `get` / `allProduct` / `size` / `empty` | $O(1)$ |
| `clear` | $O(N)$ |
| Space | $O(N)$ |

## Usage
- 総和と $0$、最小値と $\infty$、最大値と $-\infty$、GCDと $0$、行列積と単位行列、文字列連結と空文字列などが使える
- `product(left, right)` は半開区間 $[left,right)$ の積を返す
- `partitionPoint` のpredicateは単位元に対してtrueとなり、積を延長するとtrueからfalseへ単調に変化するものとする
- 範囲外のindexや不正な区間を渡すとassertionに失敗する
cpsc:text:end */

// cpsc:subsnippet:start SegmentTree
template <typename T, typename Op = std::plus<T>> class SegmentTree {
public:
  using value_type = T;
  using size_type = std::size_t;

private:
  size_type length_;
  size_type leafCount_;
  std::vector<value_type> tree_;
  [[no_unique_address]] Op op_;
  value_type unit_;

  void update(size_type index) {
    tree_[index] = op_(tree_[2 * index], tree_[2 * index + 1]);
  }

public:
  SegmentTree() : SegmentTree(0) {}

  explicit SegmentTree(size_type n, value_type unit = value_type{},
                       Op op = Op{})
      : SegmentTree(std::vector<value_type>(n, unit), unit, op) {}

  explicit SegmentTree(const std::vector<value_type> &v,
                       value_type unit = value_type{}, Op op = Op{})
      : length_(v.size()), leafCount_(std::bit_ceil(length_)),
        tree_(2 * leafCount_, unit), op_(op), unit_(unit) {
    for (size_type i = 0; i < length_; ++i) {
      tree_[leafCount_ + i] = v[i];
    }
    for (size_type i = leafCount_ - 1; i >= 1; --i) {
      update(i);
    }
  }

  void set(size_type index, const value_type &value) {
    assert(index < length_);
    index += leafCount_;
    tree_[index] = value;
    while (index > 1) {
      index >>= 1;
      update(index);
    }
  }

  [[nodiscard]] const value_type &get(size_type index) const {
    assert(index < length_);
    return tree_[index + leafCount_];
  }

  [[nodiscard]] const value_type &operator[](size_type index) const {
    return get(index);
  }

  [[nodiscard]] value_type product(size_type left, size_type right) const {
    assert(left <= right && right <= length_);
    value_type leftProduct = unit_;
    value_type rightProduct = unit_;
    size_type l = left + leafCount_;
    size_type r = right + leafCount_;

    while (l < r) {
      if (l & 1) {
        leftProduct = op_(leftProduct, tree_[l++]);
      }
      if (r & 1) {
        rightProduct = op_(tree_[--r], rightProduct);
      }
      l >>= 1;
      r >>= 1;
    }
    return op_(leftProduct, rightProduct);
  }

  [[nodiscard]] value_type allProduct() const {
    return tree_[1];
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, size_type right,
                                         P predicate) const
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));
    if (left == right) {
      return left;
    }

    size_type l = left + leafCount_;
    value_type currentProduct = unit_;

    do {
      while ((l & 1) == 0) {
        l >>= 1;
      }
      if (!predicate(op_(currentProduct, tree_[l]))) {
        while (l < leafCount_) {
          l = 2 * l;
          if (predicate(op_(currentProduct, tree_[l]))) {
            currentProduct = op_(currentProduct, tree_[l]);
            l++;
          }
        }
        return std::min(l - leafCount_, right);
      }
      currentProduct = op_(currentProduct, tree_[l]);
      l++;
    } while ((l & -l) != l);

    return right;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, P predicate) const
    requires std::predicate<P, value_type> {
    return partitionPoint(left, length_, predicate);
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type left, size_type right,
                                                P predicate) const
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));
    if (left == right) {
      return right;
    }

    size_type r = right + leafCount_;
    value_type currentProduct = unit_;

    do {
      r--;
      while (r > 1 && (r & 1)) {
        r >>= 1;
      }
      if (!predicate(op_(tree_[r], currentProduct))) {
        while (r < leafCount_) {
          r = 2 * r + 1;
          if (predicate(op_(tree_[r], currentProduct))) {
            currentProduct = op_(tree_[r], currentProduct);
            r--;
          }
        }
        return std::max(r + 1 - leafCount_, left);
      }
      currentProduct = op_(tree_[r], currentProduct);
    } while ((r & -r) != r);

    return left;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type right,
                                                P predicate) const
    requires std::predicate<P, value_type> {
    return partitionPointReverse(0, right, predicate);
  }

  void clear() {
    std::fill(tree_.begin(), tree_.end(), unit_);
  }

  [[nodiscard]] size_type size() const noexcept {
    return length_;
  }
  [[nodiscard]] bool empty() const noexcept {
    return length_ == 0;
  }
};
// cpsc:subsnippet:end
