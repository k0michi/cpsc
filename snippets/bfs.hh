/* cpsc:meta:start
slug: bfs
title: Breadth-First Search
shortTitle: BFS
category: Graph
description: 重みなしまたは辺重み0/1のグラフで、最短距離と経路を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

#include "graph-utilities.hh" // cpsc:dependency Graph Utilities

// cpsc:subsnippet:start Breadth-First Search
template <std::integral N> struct BFSResult {
  std::vector<std::optional<N>> distance;
  std::vector<std::optional<N>> previous;

  [[nodiscard]] bool reachable(N target) const noexcept {
    return distance[target].has_value();
  }

  [[nodiscard]] std::vector<N> pathTo(N target) const {
    if (!reachable(target)) {
      return {};
    }

    return restorePath(target, previous);
  }
};

template <std::integral N, typename G, std::ranges::input_range Starts>
[[nodiscard]] BFSResult<N> bfs(const G &g, const Starts &starts) {
  BFSResult<N> result{
      .distance = std::vector<std::optional<N>>(g.size()),
      .previous = std::vector<std::optional<N>>(g.size()),
  };
  std::deque<N> deque;
  for (auto &&start : starts) {
    N vertex = static_cast<N>(start);
    if (!result.distance[vertex].has_value()) {
      result.distance[vertex] = N{};
      deque.push_back(vertex);
    }
  }

  while (!deque.empty()) {
    N u = deque.front();
    deque.pop_front();
    for (auto &&edge : g[u]) {
      N v = static_cast<N>(getEdgeTarget(edge));
      bool weight = getEdgeWeightBool(edge);
      N nextDistance = *result.distance[u] + static_cast<N>(weight);
      if (result.distance[v].has_value() &&
          *result.distance[v] <= nextDistance) {
        continue;
      }
      result.distance[v] = nextDistance;
      result.previous[v] = u;
      if (weight) {
        deque.push_back(v);
      } else {
        deque.push_front(v);
      }
    }
  }
  return result;
}

// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| | Complexity |
| --- | --- |
| Time | $O(V + E)$ |
| Space | $O(V)$ |

## Usage
- 重みなしグラフでは通常のBFS、辺重みが `bool` のグラフでは0-1 BFSとして動作する
- weightを持たない辺の重みは1として扱う
- 辺は整数、`.to` と `.weight` を持つ型、あるいは `{to, weight}` のtuple-like型に対応する
- weightは `bool` へ変換し、falseを重み0、trueを重み1として扱う
- `BFSResult<N>` の `distance[v]` は始点からの最短距離、`previous[v]` は最短経路上の直前頂点を表す
- 未到達、または直前頂点がない場合は `std::nullopt`
- `reachable(target)` で到達可能性を、`pathTo(target)` で始点からの経路を取得できる
- `bfs<N>(g, starts)` のように始点をrangeで渡すと、多点始点BFSを行う
- 単一始点は1要素のrangeとして渡す
- `g[u]` は隣接コンテナへの参照だけでなく、その場で生成したコンテナを返してもよい
- 頂点番号は0以上 `g.size()` 未満の連続した整数とする
- 頂点と距離の型 `N` は `bfs<N>(g, starts)` のように明示する
cpsc:text:end */
