/* cpsc:meta:start
slug: weighted-union-find-tree
title: Weighted Union-Find Tree
shortTitle: Weighted Union-Find
category: Data Structure
description: 要素間のポテンシャル差を保ちながら集合を結合し、差分制約の整合性を管理する。
language:
  name: C++
  version: "20"
  prism: cpp
validation:
  - label: AOJ DSL_1_B
    url: https://onlinejudge.u-aizu.ac.jp/status/users/k0michi/submissions/1/DSL_1_B/judge/11668209/C++20
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| `leader` / `weight` / `unite` / `same` / `diff` / `size(x)` | Amortized $O(\alpha(N))$ |
| `clear` | $O(N)$ |
| Space | $O(N)$ |

## Usage
- `unite(x, y, w)` は $V(y)-V(x)=w$ という差分制約を追加する
- 戻り値 `{consistent, merged, leader}` は、制約が既存情報と矛盾しないか、新しく集合を結合したか、結合後の代表元を表す
- `diff(x, y)` は連結なら $V(y)-V(x)$、未連結なら `std::nullopt` を返す
- 重み型は加算、減算、単項マイナス、等値比較、加法単位元を持つものとする
- 要素は0以上 `size()` 未満の整数で表す
- 範囲外の要素を渡すとassertionに失敗する
cpsc:text:end */

// cpsc:subsnippet:start WeightedUnionFindTree
template <typename T = long long> class WeightedUnionFindTree {
public:
  using value_type = T;
  using size_type = std::size_t;

private:
  size_type elementCount_;
  size_type groupCount_;
  mutable std::vector<long long> parentOrSize_;
  mutable std::vector<value_type> diffWeight_;

public:
  WeightedUnionFindTree() : WeightedUnionFindTree(0) {}
  explicit WeightedUnionFindTree(size_type n, value_type unit = value_type{})
      : elementCount_(n), groupCount_(n), parentOrSize_(n, -1),
        diffWeight_(n, unit) {}

  size_type leader(size_type x) const {
    assert(x < elementCount_);
    if (parentOrSize_[x] < 0) {
      return x;
    }
    size_type parent = static_cast<size_type>(parentOrSize_[x]);
    size_type root = leader(parent);
    diffWeight_[x] += diffWeight_[parent];
    parentOrSize_[x] = static_cast<long long>(root);
    return root;
  }

  [[nodiscard]] value_type weight(size_type x) const {
    assert(x < elementCount_);
    leader(x);
    return diffWeight_[x];
  }

  std::tuple<bool, bool, size_type> unite(size_type x, size_type y,
                                          value_type w) {
    assert(x < elementCount_ && y < elementCount_);
    w += weight(x);
    w -= weight(y);

    size_type rootX = leader(x);
    size_type rootY = leader(y);

    if (rootX == rootY) {
      bool consistent = (w == value_type{});
      return {consistent, false, rootX};
    }

    if (-parentOrSize_[rootX] < -parentOrSize_[rootY]) {
      std::swap(rootX, rootY);
      w = -w;
    }
    parentOrSize_[rootX] += parentOrSize_[rootY];
    parentOrSize_[rootY] = static_cast<long long>(rootX);
    diffWeight_[rootY] = w;
    groupCount_--;

    return {true, true, rootX};
  }

  [[nodiscard]] bool same(size_type x, size_type y) const {
    assert(x < elementCount_ && y < elementCount_);
    return leader(x) == leader(y);
  }

  [[nodiscard]] std::optional<value_type> diff(size_type x, size_type y) const {
    assert(x < elementCount_ && y < elementCount_);
    if (!same(x, y)) {
      return std::nullopt;
    }
    return weight(y) - weight(x);
  }

  [[nodiscard]] size_type size(size_type x) const {
    assert(x < elementCount_);
    return static_cast<size_type>(-parentOrSize_[leader(x)]);
  }

  [[nodiscard]] size_type size() const noexcept {
    return elementCount_;
  }
  [[nodiscard]] size_type groupCount() const noexcept {
    return groupCount_;
  }
  [[nodiscard]] bool empty() const noexcept {
    return elementCount_ == 0;
  }

  void clear() {
    groupCount_ = elementCount_;
    std::fill(parentOrSize_.begin(), parentOrSize_.end(), -1);
    std::fill(diffWeight_.begin(), diffWeight_.end(), value_type{});
  }
};
// cpsc:subsnippet:end
