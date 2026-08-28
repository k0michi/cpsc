/* cpsc:meta:start
slug: rollback-union-find-tree
title: Rollback Union-Find Tree
shortTitle: Rollback Union-Find
category: Data Structure
description: 併合操作の履歴を保持し、以前の連結成分の状態へ巻き戻す。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start RollbackUnionFindTree
class RollbackUnionFindTree {
public:
  using size_type = std::size_t;

private:
  struct History {
    size_type rootX;
    long long parentOrSizeX;
    size_type rootY;
    long long parentOrSizeY;
  };

  size_type elementCount_;
  size_type groupCount_;
  std::vector<long long> parentOrSize_;
  std::vector<History> history_;

public:
  RollbackUnionFindTree() : RollbackUnionFindTree(0) {}

  explicit RollbackUnionFindTree(size_type n)
      : elementCount_(n), groupCount_(n), parentOrSize_(n, -1) {}

  [[nodiscard]] size_type leader(size_type x) const {
    assert(x < elementCount_);
    while (parentOrSize_[x] >= 0) {
      x = static_cast<size_type>(parentOrSize_[x]);
    }
    return x;
  }

  std::pair<bool, size_type> unite(size_type x, size_type y) {
    assert(x < elementCount_ && y < elementCount_);
    size_type rootX = leader(x);
    size_type rootY = leader(y);

    if (rootX == rootY) {
      history_.push_back(
          {rootX, parentOrSize_[rootX], rootY, parentOrSize_[rootY]});
      return {false, rootX};
    }

    if (-parentOrSize_[rootX] < -parentOrSize_[rootY]) {
      std::swap(rootX, rootY);
    }
    history_.push_back(
        {rootX, parentOrSize_[rootX], rootY, parentOrSize_[rootY]});
    parentOrSize_[rootX] += parentOrSize_[rootY];
    parentOrSize_[rootY] = static_cast<long long>(rootX);
    --groupCount_;
    return {true, rootX};
  }

  [[nodiscard]] bool same(size_type x, size_type y) const {
    assert(x < elementCount_ && y < elementCount_);
    return leader(x) == leader(y);
  }

  [[nodiscard]] size_type size(size_type x) const {
    assert(x < elementCount_);
    return static_cast<size_type>(-parentOrSize_[leader(x)]);
  }

  void undo() {
    assert(!history_.empty());
    auto [rootX, parentOrSizeX, rootY, parentOrSizeY] = history_.back();
    history_.pop_back();
    if (rootX == rootY) {
      return;
    }

    parentOrSize_[rootX] = parentOrSizeX;
    parentOrSize_[rootY] = parentOrSizeY;
    ++groupCount_;
  }

  [[nodiscard]] size_type snapshot() const noexcept {
    return history_.size();
  }

  void rollback(size_type snapshot) {
    assert(snapshot <= history_.size());
    while (history_.size() > snapshot) {
      undo();
    }
  }

  void clear() {
    groupCount_ = elementCount_;
    std::fill(parentOrSize_.begin(), parentOrSize_.end(), -1);
    history_.clear();
  }

  void resize(size_type n) {
    elementCount_ = n;
    groupCount_ = n;
    parentOrSize_.assign(n, -1);
    history_.clear();
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
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| Operation | Time | Additional space |
| --- | --- | --- |
| `leader`, `unite`, `same`, `size(x)` | $O(\log N)$ | $O(1)$ |
| `undo` | $O(1)$ | $O(1)$ |
| `rollback(snapshot)` | $O(U)$ | $O(1)$ |
| `clear`, `resize` | $O(N)$ | $O(1)$ |

$U$ は取り消す `unite` の回数。履歴全体は `unite` の呼び出し回数に比例する。

## Usage
- `unite(x, y)` は `{merged, leader}` を返し、呼び出しごとに履歴を1件追加する
- 同じ連結成分に対する冗長な `unite` も1回の `undo` に対応する
- `snapshot()` の戻り値を保存し、`rollback(snapshot)` でその時点まで巻き戻す
- rollback後に新しく `unite` すると、取り消した履歴は復元できない
- path compressionは復元する変更を増やすため行わず、union-by-sizeで親木の高さを $O(\log N)$ に抑える
- `clear` と `resize` は履歴を破棄する
cpsc:text:end */
