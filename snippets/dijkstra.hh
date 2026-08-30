/* cpsc:meta:start
slug: dijkstra
title: Dijkstra's Algorithm
shortTitle: Dijkstra
category: Graph
description: 非負コストグラフの最短距離と経路を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Dijkstra's Algorithm
template <typename E> constexpr auto getEdgeTarget(const E &e) {
  if constexpr (std::is_integral_v<std::decay_t<E>>) {
    return e;
  } else if constexpr (requires { e.to; }) {
    return e.to;
  } else if constexpr (requires { std::get<0>(e); }) {
    return std::get<0>(e);
  }
}

template <typename E> constexpr auto getEdgeCost(const E &e) {
  if constexpr (requires { e.cost; }) {
    return e.cost;
  } else if constexpr (requires { std::get<0>(e); }) {
    return std::get<1>(e);
  }
}

template <std::integral N, typename C> struct DijkstraResult {
  std::vector<std::optional<C>> distance;
  std::vector<std::optional<N>> previous;

  [[nodiscard]] bool reachable(N target) const noexcept {
    return distance[target].has_value();
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

template <std::integral N, typename C, typename G,
          std::ranges::input_range Starts>
[[nodiscard]] DijkstraResult<N, C> dijkstra(const G &g, const Starts &starts) {
  DijkstraResult<N, C> result{
      .distance = std::vector<std::optional<C>>(g.size()),
      .previous = std::vector<std::optional<N>>(g.size()),
  };
  using State = std::pair<C, N>;
  std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
  for (auto &&start : starts) {
    N vertex = static_cast<N>(start);
    if (!result.distance[vertex].has_value()) {
      result.distance[vertex] = C{};
      queue.emplace(C{}, vertex);
    }
  }

  while (!queue.empty()) {
    auto [currentDistance, vertex] = queue.top();
    queue.pop();
    if (currentDistance != result.distance[vertex]) {
      continue;
    }

    for (auto &&edge : g[vertex]) {
      N next = static_cast<N>(getEdgeTarget(edge));
      C cost = static_cast<C>(getEdgeCost(edge));
      assert(cost >= C{});
      if constexpr (std::integral<C>) {
        if (cost > std::numeric_limits<C>::max() - currentDistance) {
          continue;
        }
      }
      C nextDistance = currentDistance + cost;
      if (result.distance[next].has_value() &&
          *result.distance[next] <= nextDistance) {
        continue;
      }
      result.distance[next] = nextDistance;
      result.previous[next] = vertex;
      queue.emplace(nextDistance, next);
    }
  }
  return result;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| | Complexity |
| --- | --- |
| Time | $O((V + E) \log V)$ |
| Space | $O(V + E)$ |

## Usage
- すべての辺のcostは非負であること
- 辺は移動先とcostを `.to`, `.cost` に持つ型、または `{to, cost}` のtuple-like型に対応する
- `distance[v]` は始点からの最短距離、`previous[v]` は最短経路上の直前頂点を表す
- 未到達、または直前頂点がない場合は `std::nullopt`
- `reachable(target)` で到達可能性を、`pathTo(target)` で最短経路を取得できる
- `dijkstra<N, C>(g, starts)` のように頂点型、cost型、始点のrangeを渡す
- 多点始点では各始点の距離とpreviousを、それぞれ0と `std::nullopt` に初期化する
- 単一始点は1要素のrangeとして渡す
- `g[u]` は隣接コンテナへの参照だけでなく、その場で生成したコンテナを返してもよい
cpsc:text:end */
