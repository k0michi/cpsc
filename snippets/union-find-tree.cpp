/* cpsc:meta:start
slug: union-find-tree
title: Union-Find Tree
shortTitle: Union-Find
category: Data Structure
description: 要素を互いに素な集合へ分け、集合の結合と連結判定を高速に処理する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| `leader` / `unite` / `same` / `size(x)` | Amortized $O(\alpha(N))$ |
| `groups` / `clear` / `resize` | $O(N)$ |
| Space | $O(N)$ |

## Usage
- 要素は0以上 `size()` 未満の整数で表す
- `leader` はconst関数だが、経路圧縮のため内部状態を変更する
- `groups()` が返すグループや要素の順序に依存しない
- 範囲外の要素を渡すとassertionに失敗する
cpsc:text:end */

// cpsc:subsnippet:start UnionFindTree
class UnionFindTree {
public:
  using size_type = std::size_t;

private:
  size_type elementCount_;
  size_type groupCount_;
  mutable std::vector<long long> parentOrSize_;

public:
  UnionFindTree() : UnionFindTree(0) {}
  explicit UnionFindTree(size_type n)
      : elementCount_(n), groupCount_(n), parentOrSize_(n, -1) {}

  size_type leader(size_type x) const {
    assert(x < elementCount_);
    size_type root = x;
    while (parentOrSize_[root] >= 0) {
      root = static_cast<size_type>(parentOrSize_[root]);
    }
    while (x != root) {
      size_type parent = static_cast<size_type>(parentOrSize_[x]);
      parentOrSize_[x] = static_cast<long long>(root);
      x = parent;
    }
    return root;
  }

  bool unite(size_type x, size_type y) {
    assert(x < elementCount_ && y < elementCount_);
    size_type rootX = leader(x);
    size_type rootY = leader(y);
    if (rootX == rootY) {
      return false;
    }

    if (-parentOrSize_[rootX] < -parentOrSize_[rootY]) {
      std::swap(rootX, rootY);
    }
    parentOrSize_[rootX] += parentOrSize_[rootY];
    parentOrSize_[rootY] = static_cast<long long>(rootX);
    groupCount_--;
    return true;
  }

  [[nodiscard]] bool same(size_type x, size_type y) const {
    assert(x < elementCount_ && y < elementCount_);
    return leader(x) == leader(y);
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

  [[nodiscard]] std::vector<std::vector<size_type>> groups() const {
    std::vector<size_type> leaderBuf(elementCount_);
    std::vector<size_type> groupSize(elementCount_, 0);

    for (size_type i = 0; i < elementCount_; ++i) {
      leaderBuf[i] = leader(i);
      groupSize[leaderBuf[i]]++;
    }

    std::vector<std::vector<size_type>> result(elementCount_);
    for (size_type i = 0; i < elementCount_; ++i) {
      result[i].reserve(groupSize[i]);
    }
    for (size_type i = 0; i < elementCount_; ++i) {
      result[leaderBuf[i]].push_back(i);
    }

    result.erase(std::remove_if(result.begin(), result.end(),
                                [](const auto &g) { return g.empty(); }),
                 result.end());
    return result;
  }

  void clear() {
    groupCount_ = elementCount_;
    std::fill(parentOrSize_.begin(), parentOrSize_.end(), -1);
  }

  void resize(size_type n) {
    elementCount_ = n;
    groupCount_ = n;
    parentOrSize_.assign(n, -1);
  }

  [[nodiscard]] bool empty() const noexcept {
    return elementCount_ == 0;
  }
};
// cpsc:subsnippet:end
