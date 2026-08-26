/* cpsc:meta:start
slug: bound-helpers
title: Bound Helpers
shortTitle: Bound Helpers
category: Utility
description: 連想コンテナまたはソート済み区間から、指定値に最も近い要素を取得する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Semantics
| Function | Result |
| --- | --- |
| `floor(x)` | $x$ 以下の最大要素 |
| `lower(x)` | $x$ 未満の最大要素 |
| `ceil(x)` | $x$ 以上の最小要素 |
| `higher(x)` | $x$ より大きい最小要素 |

## Complexity
| Target | Complexity |
| --- | --- |
| Ordered associative container | $O(\log N)$ |
| Random-access range | $O(\log N)$ |

## Usage
- コンテナ版は `set`・`multiset`・`map`・`multimap` など、`lower_bound` と `upper_bound` を持つ型に使用する
- 区間版は比較関数と同じ順序でソートされた半開区間 $[first,last)$ に使用する
- `floor` と `lower` の区間版には双方向iteratorが必要
- 条件を満たす要素がない場合は `end()` または `last` を返す
cpsc:text:end */

// cpsc:subsnippet:start floor(container)
template <typename C, typename K> typename C::iterator floor(C &c, const K &x) {
  auto it = c.upper_bound(x);
  return it == c.begin() ? c.end() : std::prev(it);
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start lower(container)
template <typename C, typename K> typename C::iterator lower(C &c, const K &x) {
  auto it = c.lower_bound(x);
  return it == c.begin() ? c.end() : std::prev(it);
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceil(container)
template <typename C, typename K> typename C::iterator ceil(C &c, const K &x) {
  return c.lower_bound(x);
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start higher(container)
template <typename C, typename K>
typename C::iterator higher(C &c, const K &x) {
  return c.upper_bound(x);
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start floor(range)
template <typename F, typename T, typename C>
F floor(F first, F last, const T &value, C comp) {
  auto it = std::upper_bound(first, last, value, comp);
  return it == first ? last : std::prev(it);
}

template <typename F, typename T> F floor(F first, F last, const T &value) {
  return floor(first, last, value, std::less{});
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start lower(range)
template <typename F, typename T, typename C>
F lower(F first, F last, const T &value, C comp) {
  auto it = std::lower_bound(first, last, value, comp);
  return it == first ? last : std::prev(it);
}

template <typename F, typename T> F lower(F first, F last, const T &value) {
  return lower(first, last, value, std::less{});
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceil(range)
template <typename F, typename T, typename C>
F ceil(F first, F last, const T &value, C comp) {
  return std::lower_bound(first, last, value, comp);
}

template <typename F, typename T> F ceil(F first, F last, const T &value) {
  return ceil(first, last, value, std::less{});
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start higher(range)
template <typename F, typename T, typename C>
F higher(F first, F last, const T &value, C comp) {
  return std::upper_bound(first, last, value, comp);
}

template <typename F, typename T> F higher(F first, F last, const T &value) {
  return higher(first, last, value, std::less{});
}
// cpsc:subsnippet:end
