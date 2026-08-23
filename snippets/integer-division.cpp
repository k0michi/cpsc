/* cpsc:meta:start
slug: integer-division
title: Floor / Ceil Division
shortTitle: Floor / Ceil Div
category: Math
description: C++の0方向へ丸める整数除算を、floorまたはceil方向へ補正する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| Time | $O(1)$ |
| Space | $O(1)$ |

## Usage
- 除数 `b` は0以外であること
- `floorMod(a, b)` と `ceilMod(a, b)` はそれぞれ $a - q b$ を返す
- `std::integral`
には符号なし整数も含まれるが、負の剰余を表現する可能性がある場合は符号付き整数を使用する
- 符号付き整数の最小値を `-1` で割る操作は未定義動作となる
cpsc:text:end */

// cpsc:subsnippet:start floorDiv
template <std::integral T>
[[nodiscard]] constexpr T floorDiv(T a, T b) noexcept {
  T q = a / b;
  T r = a % b;
  bool opposite = (a < 0) ^ (b < 0);
  return (r != 0 && opposite) ? q - 1 : q;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start floorMod
template <std::integral T>
[[nodiscard]] constexpr T floorMod(T a, T b) noexcept {
  T r = a % b;
  bool opposite = (a < 0) ^ (b < 0);
  return (r != 0 && opposite) ? r + b : r;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilDiv
template <std::integral T>
[[nodiscard]] constexpr T ceilDiv(T a, T b) noexcept {
  T q = a / b;
  T r = a % b;
  bool same = (a < 0) == (b < 0);
  return (r != 0 && same) ? q + 1 : q;
}
// cpsc:subsnippet:end

// cpsc:subsnippet:start ceilMod
template <std::integral T>
[[nodiscard]] constexpr T ceilMod(T a, T b) noexcept {
  T r = a % b;
  bool same = (a < 0) == (b < 0);
  return (r != 0 && same) ? r - b : r;
}
// cpsc:subsnippet:end
