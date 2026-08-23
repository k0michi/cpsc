/* cpsc:meta:start
slug: topological-sort
title: Topological Sort
shortTitle: Topological Sort
category: Graph
description: 有向非巡回グラフの頂点を依存関係に従って並べ、閉路も検出する。
language:
  name: C++
  version: "20"
  prism: cpp
validation:
  - label: AOJ GRL_4_B
    url: https://onlinejudge.u-aizu.ac.jp/status/users/k0michi/submissions/1/GRL_4_B/judge/11666280/C++23
cpsc:meta:end */

/* cpsc:text:start
## Complexity
| Function | Complexity |
| --- | --- |
| `topologicalSort` | $O(V + E)$ |
| `lexicographicalTopologicalSort` | $O(V \log V + E)$ |
| Space | $O(V)$ |

## Usage
- 頂点番号は0以上 `g.size()` 未満の連続した整数とする
- 辺は整数、移動先を `.to`
に持つ型、または先頭要素が移動先であるtuple-like型に対応する
- 閉路が存在する場合は `std::nullopt` を返す
- `topologicalSort<int>(g)` のように、頂点数と全頂点番号を表現できる `N`
を明示する cpsc:text:end */

// cpsc:subsnippet:start Topological Sort
template <typename E> constexpr auto getEdgeTarget(const E &e) {
  if constexpr (std::is_integral_v<std::decay_t<E>>) {
    return e;
  } else if constexpr (requires { e.to; }) {
    return e.to;
  } else if constexpr (requires { std::get<0>(e); }) {
    return std::get<0>(e);
  }
}

template <std::integral N, typename G>
[[nodiscard]] std::optional<std::vector<N>> topologicalSort(const G &g) {
  N n = static_cast<N>(g.size());
  std::vector<N> inDegree(n, 0);

  for (N u = 0; u < n; ++u) {
    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      ++inDegree[v];
    }
  }

  std::queue<N> q;
  for (N u = 0; u < n; ++u) {
    if (inDegree[u] == 0) {
      q.push(u);
    }
  }

  std::vector<N> order;
  order.reserve(n);

  while (!q.empty()) {
    N u = q.front();
    q.pop();
    order.push_back(u);

    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      if (--inDegree[v] == 0) {
        q.push(v);
      }
    }
  }

  if (static_cast<N>(order.size()) != n) {
    return std::nullopt;
  }
  return order;
}

template <std::integral N, typename G>
[[nodiscard]] std::optional<std::vector<N>>
lexicographicalTopologicalSort(const G &g) {
  N n = static_cast<N>(g.size());
  std::vector<N> inDegree(n, 0);

  for (N u = 0; u < n; ++u) {
    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      ++inDegree[v];
    }
  }

  std::priority_queue<N, std::vector<N>, std::greater<N>> pq;
  for (N u = 0; u < n; ++u) {
    if (inDegree[u] == 0) {
      pq.push(u);
    }
  }

  std::vector<N> order;
  order.reserve(n);

  while (!pq.empty()) {
    N u = pq.top();
    pq.pop();
    order.push_back(u);

    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      if (--inDegree[v] == 0) {
        pq.push(v);
      }
    }
  }

  if (static_cast<N>(order.size()) != n) {
    return std::nullopt;
  }
  return order;
}
// cpsc:subsnippet:end
