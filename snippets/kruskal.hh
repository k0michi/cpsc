/* cpsc:meta:start
slug: kruskal
title: Kruskal's Algorithm
shortTitle: Kruskal
category: Graph
description: 最小全域木（MST）の辺の重みの総和と採用された辺を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

#include "graph-utilities.hh" // cpsc:dependency Graph Utilities
#include "union-find-tree.hh" // cpsc:dependency UnionFindTree

// cpsc:subsnippet:start Kruskal's Algorithm
template <typename C, typename E> struct KruskalResult {
  C totalWeight{};
  std::vector<E> edges;
  bool isConnected = false;
};

template <std::integral N, typename C, std::ranges::input_range Edges>
[[nodiscard]] KruskalResult<C, std::ranges::range_value_t<Edges>>
kruskal(N vertexCount, Edges &&edges) {
  using E = std::ranges::range_value_t<Edges>;
  using Result = KruskalResult<C, E>;
  Result result;

  if constexpr (std::signed_integral<N>) {
    assert(vertexCount >= N{});
  }
  if (vertexCount == N{}) {
    result.isConnected = true;
    return result;
  }

  std::vector<E> sortedEdges;
  if constexpr (std::ranges::sized_range<Edges>) {
    sortedEdges.reserve(std::ranges::size(edges));
  }
  for (auto &&edge : edges) {
    sortedEdges.push_back(edge);
  }
  std::ranges::sort(sortedEdges, {},
                    [](const E &edge) { return getFullEdgeCost(edge); });

  UnionFindTree unionFind(static_cast<std::size_t>(vertexCount));

  for (const E &edge : sortedEdges) {
    N source = static_cast<N>(getFullEdgeSource(edge));
    N target = static_cast<N>(getFullEdgeTarget(edge));
    assert(isValidVertex(source, vertexCount));
    assert(isValidVertex(target, vertexCount));

    if (unionFind
            .unite(static_cast<std::size_t>(source),
                   static_cast<std::size_t>(target))
            .first) {
      result.totalWeight += static_cast<C>(getFullEdgeCost(edge));
      result.edges.push_back(edge);
      if (result.edges.size() == static_cast<std::size_t>(vertexCount - 1)) {
        break;
      }
    }
  }

  result.isConnected =
      vertexCount == N{1} ||
      result.edges.size() == static_cast<std::size_t>(vertexCount - 1);
  return result;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| | Complexity |
| --- | --- |
| Time | $O(E \log E)$ |
| Space | $O(V + E)$ |

## Usage
- 辺は始点、終点、costを `.from`, `.to`, `.cost` に持つ型、または `{from, to, cost}` のtuple-like型に対応する
- `totalWeight` は最小全域木、または最小全域森の辺のcostの総和
- `edges` は最小全域木、または最小全域森を構成する辺のリスト
- `isConnected` はグラフ全体が1つの全域木で連結されたかを表す
- `kruskal<N, C>(vertexCount, edges)` のように頂点型、cost型、辺のrangeを渡す
- 頂点番号は0以上 `vertexCount` 未満の連続した整数とする
- 完全な辺のアクセサは、隣接リスト用と区別するため `getFullEdgeSource`, `getFullEdgeTarget`, `getFullEdgeCost` とする
- `UnionFindTree` code blockへ依存する
cpsc:text:end */
