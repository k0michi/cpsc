/* cpsc:meta:start
slug: dfs
title: Depth-First Search
shortTitle: DFS
category: Graph
description: 始点から深さ優先探索し、頂点を行きがけ順に列挙する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Depth-First Search
template <typename E> constexpr auto getEdgeTarget(const E &e) {
  if constexpr (std::is_integral_v<std::decay_t<E>>) {
    return e;
  } else if constexpr (requires { e.to; }) {
    return e.to;
  } else if constexpr (requires { std::get<0>(e); }) {
    return std::get<0>(e);
  }
}

template <std::integral N> struct DFSResult {
  std::vector<N> order;
  std::vector<bool> visited;
  std::vector<std::optional<N>> previous;

  [[nodiscard]] bool reachable(N target) const noexcept {
    return visited[target];
  }

  [[nodiscard]] std::vector<N> pathTo(N target) const {
    if (!reachable(target)) {
      return {};
    }

    std::vector<N> path;
    for (std::optional<N> current = target; current.has_value();
         current = previous[*current]) {
      path.push_back(*current);
    }
    std::reverse(path.begin(), path.end());
    return path;
  }
};

template <std::integral N, typename G, std::ranges::input_range Starts>
[[nodiscard]] DFSResult<N> dfs(const G &g, const Starts &starts) {
  DFSResult<N> result{
      .order = {},
      .visited = std::vector<bool>(g.size(), false),
      .previous = std::vector<std::optional<N>>(g.size()),
  };
  result.order.reserve(g.size());

  for (auto &&start : starts) {
    N vertex = static_cast<N>(start);
    if (result.visited[vertex]) {
      continue;
    }

    using State = std::pair<N, std::optional<N>>;
    std::vector<State> stack{{vertex, std::nullopt}};
    while (!stack.empty()) {
      auto [u, previous] = stack.back();
      stack.pop_back();
      if (result.visited[u]) {
        continue;
      }

      result.visited[u] = true;
      result.previous[u] = previous;
      result.order.push_back(u);

      std::vector<N> targets;
      for (auto &&edge : g[u]) {
        targets.push_back(static_cast<N>(getEdgeTarget(edge)));
      }
      for (auto it = targets.rbegin(); it != targets.rend(); ++it) {
        if (!result.visited[*it]) {
          stack.emplace_back(*it, u);
        }
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
| Space | $O(V + E)$ |

## Usage
- `order` に、始点から到達できる頂点を隣接リストの順に行きがけ順で格納する
- `visited[v]` は到達可能性、`previous[v]` はDFS木上の直前頂点を表す
- `reachable(target)` で到達可能性を、`pathTo(target)` でDFS木上の経路を取得できる
- `dfs<N>(g, starts)` のように始点をrangeで渡すと、多点始点DFSを行う
- 多点始点では、まだ訪問していない始点から順番にDFSを開始する
- 単一始点は1要素のrangeとして渡す
- 頂点番号は0以上 `g.size()` 未満の連続した整数とする
- 辺は整数、移動先を `.to` に持つ型、または先頭要素が移動先であるtuple-like型に対応する
- `g[u]` は隣接コンテナへの参照だけでなく、その場で生成したコンテナを返してもよい
- 明示的なstackを使うため、深いグラフでもcall stackを消費しない
cpsc:text:end */
