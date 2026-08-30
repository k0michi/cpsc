/* cpsc:meta:start
slug: primal-dual
title: Primal-Dual Algorithm
shortTitle: Primal-Dual
category: Graph
description: ポテンシャル付きDijkstra法を使って最小費用流を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Primal-Dual Algorithm
template <std::integral N, std::integral Cap, std::signed_integral Cost>
class PrimalDualGraph {
public:
  using EdgeIndex = std::size_t;

  struct Edge {
    N from;
    N to;
    Cap capacity;
    Cap flow;
    Cost cost;
  };

private:
  struct InternalEdge {
    N to;
    Cap capacity;
    Cost cost;
    std::size_t reverseIndex;
  };

  std::vector<std::vector<InternalEdge>> graph;
  std::vector<std::pair<N, std::size_t>> edgePositions;
  std::vector<Cost> potential;

  [[nodiscard]] static std::size_t checkedSize(N size) {
    if constexpr (std::signed_integral<N>) {
      assert(size >= N{});
    }
    return static_cast<std::size_t>(size);
  }

  [[nodiscard]] bool isValidVertex(N vertex) const noexcept {
    if constexpr (std::signed_integral<N>) {
      if (vertex < N{}) {
        return false;
      }
    }
    return static_cast<std::size_t>(vertex) < graph.size();
  }

public:
  explicit PrimalDualGraph(N vertexCount = 0)
      : graph(checkedSize(vertexCount)), potential(graph.size(), Cost{}) {}

  EdgeIndex addEdge(N from, N to, Cap capacity, Cost cost) {
    assert(isValidVertex(from));
    assert(isValidVertex(to));
    assert(capacity >= Cap{});
    assert(cost >= Cost{});

    EdgeIndex edgeIndex = edgePositions.size();
    std::size_t fromIndex = graph[from].size();
    std::size_t toIndex = graph[to].size();
    if (from == to) {
      ++toIndex;
    }
    edgePositions.emplace_back(from, fromIndex);
    graph[from].push_back({to, capacity, cost, toIndex});
    graph[to].push_back({from, Cap{}, -cost, fromIndex});
    return edgeIndex;
  }

  [[nodiscard]] Edge edge(EdgeIndex edgeIndex) const {
    assert(edgeIndex < edgePositions.size());
    auto [from, index] = edgePositions[edgeIndex];
    const InternalEdge &forward = graph[from][index];
    const InternalEdge &reverse = graph[forward.to][forward.reverseIndex];
    return {.from = from,
            .to = forward.to,
            .capacity = forward.capacity + reverse.capacity,
            .flow = reverse.capacity,
            .cost = forward.cost};
  }

  [[nodiscard]] std::size_t edgeCount() const noexcept {
    return edgePositions.size();
  }

  [[nodiscard]] N vertexCount() const noexcept {
    return static_cast<N>(graph.size());
  }

  void setCapacity(EdgeIndex edgeIndex, Cap newCapacity) {
    assert(edgeIndex < edgePositions.size());
    auto [from, index] = edgePositions[edgeIndex];
    InternalEdge &forward = graph[from][index];
    const InternalEdge &reverse = graph[forward.to][forward.reverseIndex];
    assert(newCapacity >= reverse.capacity);
    forward.capacity = newCapacity - reverse.capacity;
  }

  void clearFlow() {
    for (auto [from, index] : edgePositions) {
      InternalEdge &forward = graph[from][index];
      InternalEdge &reverse = graph[forward.to][forward.reverseIndex];
      forward.capacity += reverse.capacity;
      reverse.capacity = Cap{};
    }
    std::fill(potential.begin(), potential.end(), Cost{});
  }

  [[nodiscard]] std::vector<std::pair<Cap, Cost>>
  slope(N source, N sink, Cap flowLimit = std::numeric_limits<Cap>::max()) {
    assert(isValidVertex(source));
    assert(isValidVertex(sink));
    assert(source != sink);

    Cap currentFlow{};
    Cost currentCost{};
    std::vector<std::pair<Cap, Cost>> result{{currentFlow, currentCost}};
    std::optional<Cost> previousCostPerFlow;
    const Cost infinity = std::numeric_limits<Cost>::max();
    std::vector<Cost> distance(graph.size());
    std::vector<N> previousVertex(graph.size());
    std::vector<std::size_t> previousEdge(graph.size());
    using State = std::pair<Cost, N>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;

    while (currentFlow < flowLimit) {
      std::fill(distance.begin(), distance.end(), infinity);
      distance[source] = Cost{};
      queue.emplace(Cost{}, source);
      while (!queue.empty()) {
        auto [currentDistance, vertex] = queue.top();
        queue.pop();
        if (distance[vertex] < currentDistance) {
          continue;
        }

        for (std::size_t index = 0; index < graph[vertex].size(); ++index) {
          const InternalEdge &edge = graph[vertex][index];
          if (edge.capacity <= Cap{}) {
            continue;
          }
          Cost reducedCost = edge.cost + potential[vertex] - potential[edge.to];
          if (distance[edge.to] > distance[vertex] + reducedCost) {
            distance[edge.to] = distance[vertex] + reducedCost;
            previousVertex[edge.to] = vertex;
            previousEdge[edge.to] = index;
            queue.emplace(distance[edge.to], edge.to);
          }
        }
      }
      if (distance[sink] == infinity) {
        break;
      }

      for (N vertex = 0; vertex < static_cast<N>(graph.size()); ++vertex) {
        if (distance[vertex] != infinity) {
          potential[vertex] += distance[vertex];
        }
      }

      Cap additionalFlow = flowLimit - currentFlow;
      for (N vertex = sink; vertex != source; vertex = previousVertex[vertex]) {
        additionalFlow = std::min(
            additionalFlow,
            graph[previousVertex[vertex]][previousEdge[vertex]].capacity);
      }
      for (N vertex = sink; vertex != source; vertex = previousVertex[vertex]) {
        InternalEdge &edge =
            graph[previousVertex[vertex]][previousEdge[vertex]];
        edge.capacity -= additionalFlow;
        graph[vertex][edge.reverseIndex].capacity += additionalFlow;
      }

      Cost costPerFlow = potential[sink];
      currentFlow += additionalFlow;
      currentCost += static_cast<Cost>(additionalFlow) * costPerFlow;
      if (previousCostPerFlow == costPerFlow) {
        result.pop_back();
      }
      result.emplace_back(currentFlow, currentCost);
      previousCostPerFlow = costPerFlow;
    }
    return result;
  }

  [[nodiscard]] std::pair<Cap, Cost>
  flow(N source, N sink, Cap flowLimit = std::numeric_limits<Cap>::max()) {
    return slope(source, sink, flowLimit).back();
  }
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| | Complexity |
| --- | --- |
| Time | $O(F E \log V)$ |
| Space | $O(V + E)$ |

$F$ は増加路を見つける反復回数を表す。

## Usage
- `PrimalDualGraph<N, Cap, Cost> graph(vertexCount)` のように頂点型、容量型、費用型を指定する
- `addEdge(from, to, capacity, cost)` は辺を追加し、`EdgeIndex` を返す
- `edge(edgeIndex)` は辺の始点、終点、容量、現在流量、費用を `PrimalDualGraph::Edge` として返す
- `edgeCount()` と `vertexCount()` で辺数と頂点数を取得できる
- `setCapacity(edgeIndex, capacity)` で現在流量以上の新しい容量を設定できる
- `clearFlow()` は全辺の流量を0に戻し、設定された容量を維持する
- 容量と費用は0以上とする。負費用辺を扱うには初期ポテンシャルの計算が別途必要
- `flow(source, sink, flowLimit)` は `{流量, 総費用}` を返す
- `slope(source, sink, flowLimit)` は `{累積流量, 累積費用}` の折れ点列を返す
- `flow` と `slope` は残余グラフとポテンシャルを更新し、連続呼び出しでは追加分の流量と費用を返す
- `clearFlow()` 後は初期状態から再計算できる
cpsc:text:end */
