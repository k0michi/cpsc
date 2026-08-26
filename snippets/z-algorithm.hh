/* cpsc:meta:start
slug: z-algorithm
title: Z Algorithm
shortTitle: Z Algorithm
category: String
description: 各位置から始まる列と列全体の最長共通接頭辞長を線形時間で求める。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

/* cpsc:text:start
## Semantics

長さ $N$ の列 $S$ に対し、`z(S)[i]`は $S$ と $S[i,N)$ の最長共通接頭辞長を表す。先頭要素は `z(S)[0] = N` とする。

## Complexity
| Operation | Time | Space |
| --- | --- | --- |
| `z(first, last)` | $O(N)$ | $O(N)$ |
| `z(range)` | $O(N)$ | $O(N)$ |

## Usage
- 文字列だけでなく、等値比較できる要素を持つrandom-access rangeに使用できる
- 空のrangeには空のvectorを返す
- iterator版には同じrangeを指すrandom-access iteratorの半開区間 $[first,last)$ を渡す
- pattern、区切り、textを連結して適用すると、各位置でのpatternとの一致長を求められる
cpsc:text:end */

// cpsc:subsnippet:start z
template <std::random_access_iterator I>
[[nodiscard]] constexpr std::vector<std::size_t> z(I first, I last) {
  std::size_t n = static_cast<std::size_t>(last - first);
  if (n == 0) {
    return {};
  }

  std::vector<std::size_t> result(n);
  result[0] = n;
  std::size_t left = 0;
  std::size_t right = 0;

  for (std::size_t i = 1; i < n; ++i) {
    if (i < right) {
      result[i] = std::min(right - i, result[i - left]);
    }
    while (i + result[i] < n && first[result[i]] == first[i + result[i]]) {
      ++result[i];
    }
    if (i + result[i] > right) {
      left = i;
      right = i + result[i];
    }
  }
  return result;
}

template <typename R>
  requires std::ranges::random_access_range<const R> &&
           std::ranges::common_range<const R>
[[nodiscard]] constexpr std::vector<std::size_t> z(const R &range) {
  return z(std::ranges::begin(range), std::ranges::end(range));
}
// cpsc:subsnippet:end
