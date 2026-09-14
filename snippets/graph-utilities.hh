/* cpsc:meta:start
slug: graph-utilities
title: Graph Utilities
shortTitle: Graph Utilities
category: Graph
description: グラフアルゴリズムで共通して使う辺アクセサ、頂点検証、経路復元を提供する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Graph Utilities
template <typename E> constexpr auto getEdgeTarget(const E &edge) {
  if constexpr (std::is_integral_v<std::decay_t<E>>) {
    return edge;
  } else if constexpr (requires { edge.to; }) {
    return edge.to;
  } else if constexpr (requires { std::get<0>(edge); }) {
    return std::get<0>(edge);
  }
}

template <typename E> constexpr auto getEdgeCost(const E &edge) {
  if constexpr (requires { edge.cost; }) {
    return edge.cost;
  } else if constexpr (requires { std::get<1>(edge); }) {
    return std::get<1>(edge);
  }
}

template <typename E> constexpr bool getEdgeWeightBool(const E &edge) {
  if constexpr (std::is_integral_v<std::decay_t<E>>) {
    return true;
  } else if constexpr (requires { edge.weight; }) {
    return static_cast<bool>(edge.weight);
  } else if constexpr (requires { std::get<1>(edge); }) {
    return static_cast<bool>(std::get<1>(edge));
  } else {
    return true;
  }
}

template <typename E> constexpr auto getFullEdgeSource(const E &edge) {
  if constexpr (requires { edge.from; }) {
    return edge.from;
  } else if constexpr (requires { std::get<0>(edge); }) {
    return std::get<0>(edge);
  }
}

template <typename E> constexpr auto getFullEdgeTarget(const E &edge) {
  if constexpr (requires { edge.to; }) {
    return edge.to;
  } else if constexpr (requires { std::get<1>(edge); }) {
    return std::get<1>(edge);
  }
}

template <typename E> constexpr auto getFullEdgeCost(const E &edge) {
  if constexpr (requires { edge.cost; }) {
    return edge.cost;
  } else if constexpr (requires { std::get<2>(edge); }) {
    return std::get<2>(edge);
  }
}

template <std::integral N, std::integral S>
[[nodiscard]] constexpr bool isValidVertex(N vertex, S vertexCount) noexcept {
  return !std::cmp_less(vertex, 0) && std::cmp_less(vertex, vertexCount);
}

template <std::integral N>
[[nodiscard]] std::vector<N>
restorePath(N target, const std::vector<std::optional<N>> &previous) {
  std::vector<N> path;
  for (std::optional<N> current = target; current.has_value();
       current = previous[*current]) {
    path.push_back(*current);
  }
  std::reverse(path.begin(), path.end());
  return path;
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Edge interfaces

隣接listの辺には`getEdgeTarget`を使う。辺は頂点番号そのもの、`.to`を持つ型、または`{to, ...}`のtuple-like型で表す。

重み付き隣接listでは`getEdgeCost`が`.cost`またはtuple-like型の第2要素を返す。0-1 BFS用の`getEdgeWeightBool`は`.weight`または第2要素を`bool`へ変換し、頂点番号だけの辺は重み1として扱う。

辺listの完全な辺には`getFullEdgeSource`、`getFullEdgeTarget`、`getFullEdgeCost`を使う。`.from`, `.to`, `.cost`を持つ型、または`{from, to, cost}`のtuple-like型に対応する。

## Other utilities

- `isValidVertex(vertex, vertexCount)`は符号の異なる整数型を安全に比較し、$0\le vertex<vertexCount$か判定する
- `restorePath(target, previous)`は`previous`を根まで辿り、根から`target`までの頂点列を返す

`restorePath`の時間・返り値を含む空間計算量は経路長を$L$として$O(L)$、その他の関数は時間・空間ともに$O(1)$である。
cpsc:text:end */
