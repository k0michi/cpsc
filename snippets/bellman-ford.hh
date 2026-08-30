/* cpsc:meta:start
slug: bellman-ford
title: Bellman-Ford's Algorithm
shortTitle: Bellman-Ford
category: Graph
description: 負辺を含むグラフの最短距離、負閉路検出、経路復元を行う。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Bellman-Ford's Algorithm
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

template <std::integral N, typename C> struct BellmanFordResult {
  std::vector<std::optional<C>> distance;
  std::vector<std::optional<N>> previous;
  bool hasNegativeCycle = false;
  std::vector<bool> hasNegativeCycleTo;

  [[nodiscard]] bool reachable(N target) const noexcept {
    return distance[target].has_value();
  }

  [[nodiscard]] bool isNegativeInfinity(N target) const noexcept {
    return hasNegativeCycleTo[target];
  }

  [[nodiscard]] std::vector<N> pathTo(N target) const {
    if (!reachable(target) || isNegativeInfinity(target)) {
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
[[nodiscard]] BellmanFordResult<N, C> bellmanFord(const G &g,
                                                  const Starts &starts) {
  auto addCost = [](C left, C right) -> std::optional<C> {
    if constexpr (std::integral<C>) {
      if (right > C{} && left > std::numeric_limits<C>::max() - right) {
        return std::nullopt;
      }
      if (right < C{} && left < std::numeric_limits<C>::lowest() - right) {
        return std::nullopt;
      }
    }
    return left + right;
  };

  const std::size_t vertexCount = g.size();
  BellmanFordResult<N, C> result{
      .distance = std::vector<std::optional<C>>(vertexCount),
      .previous = std::vector<std::optional<N>>(vertexCount),
      .hasNegativeCycle = false,
      .hasNegativeCycleTo = std::vector<bool>(vertexCount, false),
  };

  for (auto &&start : starts) {
    N vertex = static_cast<N>(start);
    if (!result.distance[vertex].has_value()) {
      result.distance[vertex] = C{};
    }
  }

  for (std::size_t iteration = 0; iteration + 1 < vertexCount; ++iteration) {
    bool updated = false;
    for (N vertex = 0; vertex < static_cast<N>(vertexCount); ++vertex) {
      if (!result.distance[vertex].has_value()) {
        continue;
      }
      for (auto &&edge : g[vertex]) {
        N next = static_cast<N>(getEdgeTarget(edge));
        C cost = static_cast<C>(getEdgeCost(edge));
        std::optional<C> nextDistance = addCost(*result.distance[vertex], cost);
        if (nextDistance.has_value() &&
            (!result.distance[next].has_value() ||
             *result.distance[next] > *nextDistance)) {
          result.distance[next] = *nextDistance;
          result.previous[next] = vertex;
          updated = true;
        }
      }
    }
    if (!updated) {
      return result;
    }
  }

  std::queue<N> queue;
  for (N vertex = 0; vertex < static_cast<N>(vertexCount); ++vertex) {
    if (!result.distance[vertex].has_value()) {
      continue;
    }
    for (auto &&edge : g[vertex]) {
      N next = static_cast<N>(getEdgeTarget(edge));
      C cost = static_cast<C>(getEdgeCost(edge));
      std::optional<C> nextDistance = addCost(*result.distance[vertex], cost);
      if (nextDistance.has_value() &&
          (!result.distance[next].has_value() ||
           *result.distance[next] > *nextDistance) &&
          !result.hasNegativeCycleTo[next]) {
        result.hasNegativeCycleTo[next] = true;
        queue.push(next);
      }
    }
  }

  result.hasNegativeCycle = !queue.empty();
  while (!queue.empty()) {
    N vertex = queue.front();
    queue.pop();
    for (auto &&edge : g[vertex]) {
      N next = static_cast<N>(getEdgeTarget(edge));
      if (!result.hasNegativeCycleTo[next]) {
        result.hasNegativeCycleTo[next] = true;
        queue.push(next);
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
| Time | $O(V \cdot E)$ |
| Space | $O(V)$ |

## Usage
- 負のcostを持つ辺を含む有向・無向グラフの最短経路を求める
- 辺は移動先とcostを `.to`, `.cost` に持つ型、または `{to, cost}` のtuple-like型に対応する
- `distance[v]` は始点からの最短距離、`previous[v]` は最短経路上の直前頂点を表す
- 未到達、または直前頂点がない場合は `std::nullopt`
- `hasNegativeCycle` は始点から到達可能な負閉路が存在するかを表す
- `isNegativeInfinity(target)` は負閉路を経由してtargetの距離をいくらでも小さくできるかを表す
- `pathTo(target)` は最短経路を返す。未到達または負閉路の影響下では空配列を返す
- `bellmanFord<N, C>(g, starts)` のように頂点型、cost型、始点のrangeを渡す
- 多点始点では各始点の距離を0に初期化する
- 単一始点は1要素のrangeとして渡す
- `g[u]` は隣接コンテナへの参照だけでなく、その場で生成したコンテナを返してもよい
cpsc:text:end */
