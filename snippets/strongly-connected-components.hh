/* cpsc:meta:start
slug: strongly-connected-components
title: Strongly Connected Components
shortTitle: SCC
category: Graph
description: 有向グラフを、互いに到達可能な頂点からなる強連結成分へ分解する。
language:
  name: C++
  version: "20"
  prism: cpp
validation:
  - label: AtCoder typical90_u
    url: https://atcoder.jp/contests/typical90/submissions/78663056
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Algorithm
Tarjan法を使う。DFSで各頂点の訪問順 `ord`
と、現在のDFS木から到達できる最小の訪問順 `low` を管理する。`ord[u] == low[u]`
となる頂点を根として、stack上の頂点を1つの強連結成分へまとめる。

Tarjan法ではシンク側の成分から確定するため、最後に成分列を反転し、縮約後のDAGに対するトポロジカル順で返す。

## Complexity
| | Complexity |
| --- | --- |
| Time | $O(V + E)$ |
| Space | $O(V)$ |

## Usage
- 頂点番号は0以上 `g.size()` 未満の連続した整数とする
- 辺は整数、移動先を `.to`
に持つ型、または先頭要素が移動先であるtuple-like型に対応する
- `stronglyConnectedComponents<int>(g)` のように頂点型 `N` を明示する
- 各成分内の頂点順には依存しない
- 再帰DFSを使うため、非常に深いグラフではcall stackに注意する
cpsc:text:end */

// cpsc:subsnippet:start Strongly Connected Components
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
[[nodiscard]] std::vector<std::vector<N>>
stronglyConnectedComponents(const G &g) {
  N n = static_cast<N>(g.size());
  N nowOrd = 0;
  std::vector<N> ord(n, -1), low(n, -1);
  std::vector<bool> inStack(n, false);
  std::vector<N> stack;
  stack.reserve(n);

  std::vector<std::vector<N>> groups;

  auto dfs = [&](auto &self, N u) -> void {
    ord[u] = low[u] = nowOrd++;
    stack.push_back(u);
    inStack[u] = true;

    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      if (ord[v] == -1) {
        self(self, v);
        low[u] = std::min(low[u], low[v]);
      } else if (inStack[v]) {
        low[u] = std::min(low[u], ord[v]);
      }
    }

    if (ord[u] == low[u]) {
      std::vector<N> group;
      while (true) {
        N v = stack.back();
        stack.pop_back();
        inStack[v] = false;
        group.push_back(v);
        if (u == v) {
          break;
        }
      }
      groups.push_back(std::move(group));
    }
  };

  for (N i = 0; i < n; ++i) {
    if (ord[i] == -1) {
      dfs(dfs, i);
    }
  }

  std::reverse(groups.begin(), groups.end());
  return groups;
}
// cpsc:subsnippet:end
