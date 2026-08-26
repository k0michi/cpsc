/* cpsc:meta:start
slug: fenwick-tree
title: Fenwick Tree
shortTitle: Fenwick Tree
category: Data Structure
description: 要素の更新と累積和・区間和を対数時間で処理する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start FenwickTree
// cpsc:subsnippet:description 一点更新、区間和、累積和に対する境界探索を提供する。
template <typename T = long long> class FenwickTree {
public:
  using value_type = T;
  using size_type = std::size_t;

private:
  size_type length_;
  std::vector<value_type> tree_;

public:
  FenwickTree() : FenwickTree(0) {}
  explicit FenwickTree(size_type n) : length_(n), tree_(n + 1, value_type{}) {}

  explicit FenwickTree(const std::vector<value_type> &v)
      : length_(v.size()), tree_(v.size() + 1, value_type{}) {
    for (size_type i = 0; i < length_; ++i) {
      tree_[i + 1] = v[i];
    }
    for (size_type i = 1; i <= length_; ++i) {
      size_type parent = i + (i & -i);
      if (parent <= length_) {
        tree_[parent] += tree_[i];
      }
    }
  }

  void add(size_type index, const value_type &delta) {
    assert(index < length_);
    for (size_type i = index + 1; i <= length_; i += i & -i) {
      tree_[i] += delta;
    }
  }

  void set(size_type index, const value_type &value) {
    assert(index < length_);
    add(index, value - get(index));
  }

  [[nodiscard]] value_type sum(size_type right) const {
    assert(right <= length_);
    value_type res{};
    for (size_type i = right; i > 0; i -= i & -i) {
      res += tree_[i];
    }
    return res;
  }

  [[nodiscard]] value_type sum(size_type left, size_type right) const {
    assert(left <= right && right <= length_);
    return sum(right) - sum(left);
  }

  [[nodiscard]] value_type get(size_type index) const {
    assert(index < length_);
    return sum(index, index + 1);
  }

  [[nodiscard]] value_type operator[](size_type index) const {
    return get(index);
  }

  [[nodiscard]] size_type lowerBound(value_type value) const {
    if (value <= value_type{}) {
      return 0;
    }
    size_type index = 0;
    for (size_type i = std::bit_floor(length_); i > 0; i >>= 1) {
      if (index + i <= length_ && tree_[index + i] < value) {
        value -= tree_[index + i];
        index += i;
      }
    }
    return index;
  }

  [[nodiscard]] size_type upperBound(value_type value) const {
    if (value < value_type{}) {
      return 0;
    }
    size_type index = 0;
    for (size_type i = std::bit_floor(length_); i > 0; i >>= 1) {
      if (index + i <= length_ && tree_[index + i] <= value) {
        value -= tree_[index + i];
        index += i;
      }
    }
    return index;
  }

  void clear() {
    std::fill(tree_.begin(), tree_.end(), value_type{});
  }

  [[nodiscard]] size_type size() const noexcept {
    return length_;
  }
  [[nodiscard]] bool empty() const noexcept {
    return length_ == 0;
  }
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N)$ |
| `add` / `set` / `sum` / `get` | $O(\log N)$ |
| `lowerBound` / `upperBound` | $O(\log N)$ |
| Space | $O(N)$ |

## Usage
- 添字は0-indexed、`sum(left, right)` は半開区間 $[left, right)$ の総和を返す
- `lowerBound`・`upperBound` は累積和が単調非減少となる場合に限り使用する。通常は全要素が非負であることが必要
- 境界探索で条件を満たす位置がない場合は `size()` を返す
- 範囲外の添字はassertionに失敗する
cpsc:text:end */
