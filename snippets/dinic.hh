/* cpsc:meta:start
slug: dinic
title: Dinic's Algorithm
shortTitle: Dinic
category: Graph
description: レベルグラフとブロッキングフローを使って最大流を求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Dinic's Algorithm
template <std::integral N, std::integral C> class DinicGraph {
public:
  using EdgeIndex = std::size_t;

  struct Edge {
    N from;
    N to;
    C capacity;
    C flow;
  };

private:
  using Level = std::make_signed_t<N>;

  struct InternalEdge {
    N to;
    C capacity;
    std::size_t reverseIndex;
  };

  std::vector<std::vector<InternalEdge>> graph;
  std::vector<Level> level;
  std::vector<std::size_t> iter;
  std::vector<std::pair<N, std::size_t>> edgePositions;

  bool buildLevelGraph(N source, N sink) {
    std::fill(level.begin(), level.end(), -1);
    std::queue<N> queue;
    level[source] = 0;
    queue.push(source);

    while (!queue.empty()) {
      N vertex = queue.front();
      queue.pop();
      for (const InternalEdge &edge : graph[vertex]) {
        if (edge.capacity > C{} && level[edge.to] == -1) {
          level[edge.to] = level[vertex] + 1;
          queue.push(edge.to);
        }
      }
    }
    return level[sink] != -1;
  }

  C sendFlow(N vertex, N sink, C flow) {
    if (vertex == sink) {
      return flow;
    }

    for (std::size_t &i = iter[vertex]; i < graph[vertex].size(); ++i) {
      InternalEdge &edge = graph[vertex][i];
      if (edge.capacity <= C{} || level[edge.to] != level[vertex] + 1) {
        continue;
      }

      C sent = sendFlow(edge.to, sink, std::min(flow, edge.capacity));
      if (sent > C{}) {
        edge.capacity -= sent;
        graph[edge.to][edge.reverseIndex].capacity += sent;
        return sent;
      }
    }
    return C{};
  }

public:
  explicit DinicGraph(N vertexCount = 0)
      : graph(vertexCount), level(vertexCount), iter(vertexCount) {}

  EdgeIndex addEdge(N from, N to, C capacity) {
    assert(from < static_cast<N>(graph.size()));
    assert(to < static_cast<N>(graph.size()));
    assert(capacity >= C{});

    EdgeIndex edgeIndex = edgePositions.size();
    std::size_t fromIndex = graph[from].size();
    std::size_t toIndex = graph[to].size();
    if (from == to) {
      ++toIndex;
    }
    edgePositions.emplace_back(from, fromIndex);
    graph[from].push_back({to, capacity, toIndex});
    graph[to].push_back({from, C{}, fromIndex});
    return edgeIndex;
  }

  [[nodiscard]] Edge edge(EdgeIndex edgeIndex) const {
    assert(edgeIndex < edgePositions.size());
    auto [from, index] = edgePositions[edgeIndex];
    const InternalEdge &forward = graph[from][index];
    const InternalEdge &reverse = graph[forward.to][forward.reverseIndex];
    return {
        .from = from,
        .to = forward.to,
        .capacity = forward.capacity + reverse.capacity,
        .flow = reverse.capacity,
    };
  }

  [[nodiscard]] std::size_t edgeCount() const noexcept {
    return edgePositions.size();
  }

  [[nodiscard]] N vertexCount() const noexcept {
    return static_cast<N>(graph.size());
  }

  void setCapacity(EdgeIndex edgeIndex, C newCapacity) {
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
      reverse.capacity = C{};
    }
  }

  [[nodiscard]] C flow(N source, N sink,
                       C flowLimit = std::numeric_limits<C>::max()) {
    if (source == sink) {
      return C{};
    }

    C totalFlow{};
    while (totalFlow < flowLimit && buildLevelGraph(source, sink)) {
      std::fill(iter.begin(), iter.end(), 0);
      while (totalFlow < flowLimit) {
        C sent = sendFlow(source, sink, flowLimit - totalFlow);
        if (sent == C{}) {
          break;
        }
        totalFlow += sent;
      }
    }
    return totalFlow;
  }

  [[nodiscard]] std::vector<bool> minCut(N source) const {
    assert(source < static_cast<N>(graph.size()));
    std::vector<bool> visited(graph.size(), false);
    std::queue<N> queue;
    visited[source] = true;
    queue.push(source);

    while (!queue.empty()) {
      N vertex = queue.front();
      queue.pop();
      for (const InternalEdge &edge : graph[vertex]) {
        if (edge.capacity > C{} && !visited[edge.to]) {
          visited[edge.to] = true;
          queue.push(edge.to);
        }
      }
    }
    return visited;
  }
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| | Complexity |
| --- | --- |
| Time | $O(V^2 E)$ |
| Space | $O(V + E)$ |

## Usage
- `DinicGraph<N, C> dinic(vertexCount)` のように頂点型と容量型を指定する
- `addEdge(from, to, capacity)` は容量付き有向辺を追加し、`EdgeIndex` を返す
- `edge(edgeIndex)` は辺の始点、終点、容量、現在流量を `DinicGraph::Edge` として返す
- `edgeCount()` と `vertexCount()` で辺数と頂点数を取得できる
- `setCapacity(edgeIndex, capacity)` で現在流量以上の新しい容量を設定できる
- `clearFlow()` は全辺の流量を0に戻し、設定された容量を維持する
- 容量は0以上とする
- `flow(source, sink)` は残余グラフ上で最大流を追加し、その呼び出しで流した量を返す
- 第3引数を指定すると、その値を上限として流す
- `minCut(source)` は残余グラフでsourceから到達可能な頂点をtrueとする配列を返す
- 頂点番号は0以上 `vertexCount` 未満の連続した整数とする
- `sendFlow` は再帰するため、非常に深いレベルグラフではcall stackに注意する
cpsc:text:end */
