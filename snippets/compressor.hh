/* cpsc:meta:start
slug: compressor
title: Coordinate Compressor
shortTitle: Compressor
category: Data Structure
description: 1次元座標をソート・重複除去し、連続したindexへ写像する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start Compressor
template <typename T = long long> class Compressor {
public:
  using value_type = T;
  using size_type = std::size_t;

  explicit Compressor(std::vector<value_type> values)
      : values_(std::move(values)) {
    std::ranges::sort(values_);
    auto [first, last] = std::ranges::unique(values_);
    values_.erase(first, last);
  }

  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_reference_t<R>, value_type>
  explicit Compressor(R &&range)
      : Compressor(toVector(std::forward<R>(range))) {}

  Compressor(std::initializer_list<value_type> values)
      : Compressor(std::vector<value_type>(values)) {}

  [[nodiscard]] size_type get(const value_type &value) const noexcept {
    return static_cast<size_type>(std::ranges::lower_bound(values_, value) -
                                  values_.begin());
  }

  [[nodiscard]] size_type operator()(const value_type &value) const noexcept {
    return get(value);
  }

  [[nodiscard]] const value_type &operator[](size_type index) const noexcept {
    assert(index < size());
    return values_[index];
  }

  [[nodiscard]] value_type stepWidth(size_type index) const noexcept {
    assert(index + 1 < size());
    return values_[index + 1] - values_[index];
  }

  [[nodiscard]] size_type size() const noexcept {
    return values_.size();
  }

  [[nodiscard]] bool empty() const noexcept {
    return values_.empty();
  }

  [[nodiscard]] const std::vector<value_type> &values() const noexcept {
    return values_;
  }

private:
  template <std::ranges::input_range R>
  static std::vector<value_type> toVector(R &&range) {
    std::vector<value_type> values;
    if constexpr (std::ranges::sized_range<R>) {
      values.reserve(std::ranges::size(range));
    }
    for (auto &&value : range) {
      values.emplace_back(std::forward<decltype(value)>(value));
    }
    return values;
  }

  std::vector<value_type> values_;
};

template <std::ranges::input_range R>
Compressor(R &&) -> Compressor<std::ranges::range_value_t<R>>;

template <typename T> Compressor(std::initializer_list<T>) -> Compressor<T>;
// cpsc:subsnippet:end

/* cpsc:text:start
## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N\log N)$ |
| `get` | $O(\log N)$ |
| `operator[]`, `stepWidth` | $O(1)$ |
| Space | $O(N)$ |

## Usage
- `get(value)`は`value`の挿入位置を返す。構築時に渡していない値にも使用できる
- `operator[](index)`で圧縮後のindexから元の座標へ戻せる
- `stepWidth(index)`は隣接する座標間の幅を返すため、`index + 1 < size()`が必要
cpsc:text:end */
