/* cpsc:meta:start
slug: trie
title: Trie
shortTitle: Trie
category: String
description: 列をprefixで共有して格納し、完全一致とprefix一致を列長に比例する時間で処理する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start AsciiCharacterToIndex / Trie
template <char B = 'a'> struct AsciiCharacterToIndex {
  [[nodiscard]] constexpr std::size_t
  operator()(char character) const noexcept {
    return static_cast<std::size_t>(character - B);
  }
};

template <char B = 'a'> struct AsciiIndexToCharacter {
  [[nodiscard]] constexpr char operator()(std::size_t index) const noexcept {
    return static_cast<char>(B + index);
  }
};

template <std::size_t A = 26, typename F = AsciiCharacterToIndex<'a'>,
          typename G = AsciiIndexToCharacter<'a'>>
class Trie {
public:
  using size_type = std::size_t;

  static constexpr size_type AlphabetSize = A;
  static constexpr size_type Null = static_cast<size_type>(-1);

  struct Node {
    std::array<size_type, AlphabetSize> next;
    size_type terminalCount = 0;
    size_type prefixCount = 0;

    constexpr Node() noexcept {
      next.fill(Null);
    }
  };

  explicit Trie(F characterToIndex = F{}, G indexToElement = G{})
      : characterToIndex_(std::move(characterToIndex)),
        indexToElement_(std::move(indexToElement)) {
    clear();
  }

  template <std::ranges::input_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  void insert(const R &sequence, size_type multiplicity = 1) {
    if (multiplicity == 0) {
      return;
    }

    size_type current = 0;
    nodes_[current].prefixCount += multiplicity;
    for (auto &&element : sequence) {
      size_type index =
          static_cast<size_type>(std::invoke(characterToIndex_, element));
      assert(index < AlphabetSize);
      if (nodes_[current].next[index] == Null) {
        nodes_[current].next[index] = createNode();
      }
      current = nodes_[current].next[index];
      nodes_[current].prefixCount += multiplicity;
    }
    nodes_[current].terminalCount += multiplicity;
    wordCount_ += multiplicity;
  }

  template <std::ranges::forward_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  size_type erase(const R &sequence, size_type multiplicity = 1) {
    if (multiplicity == 0 || empty()) {
      return 0;
    }

    size_type terminal = findNode(sequence);
    if (terminal == Null || nodes_[terminal].terminalCount == 0) {
      return 0;
    }
    size_type erased = std::min(multiplicity, nodes_[terminal].terminalCount);

    size_type current = 0;
    nodes_[current].prefixCount -= erased;
    for (auto &&element : sequence) {
      size_type index =
          static_cast<size_type>(std::invoke(characterToIndex_, element));
      current = nodes_[current].next[index];
      nodes_[current].prefixCount -= erased;
    }
    nodes_[current].terminalCount -= erased;
    wordCount_ -= erased;
    return erased;
  }

  template <std::ranges::input_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  [[nodiscard]] size_type findNode(const R &sequence) const {
    size_type current = 0;
    for (auto &&element : sequence) {
      size_type index =
          static_cast<size_type>(std::invoke(characterToIndex_, element));
      if (index >= AlphabetSize || nodes_[current].next[index] == Null) {
        return Null;
      }
      current = nodes_[current].next[index];
    }
    return current;
  }

  template <std::ranges::input_range R>
  [[nodiscard]] size_type count(const R &sequence) const {
    size_type index = findNode(sequence);
    return index == Null ? 0 : nodes_[index].terminalCount;
  }

  template <std::ranges::input_range R>
  [[nodiscard]] bool contains(const R &sequence) const {
    return count(sequence) > 0;
  }

  template <std::ranges::input_range R>
  [[nodiscard]] size_type countPrefix(const R &prefix) const {
    size_type index = findNode(prefix);
    return index == Null ? 0 : nodes_[index].prefixCount;
  }

  template <std::ranges::input_range R>
  [[nodiscard]] bool containsPrefix(const R &prefix) const {
    return countPrefix(prefix) > 0;
  }

  template <std::ranges::input_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  [[nodiscard]] size_type countLess(const R &sequence) const {
    return lexicographicalCounts(sequence).first;
  }

  template <std::ranges::input_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  [[nodiscard]] size_type countLessEqual(const R &sequence) const {
    return lexicographicalCounts(sequence).second;
  }

  template <typename C>
    requires std::default_initializable<C> &&
             std::invocable<const G &, size_type> &&
             requires(C &container,
                      std::invoke_result_t<const G &, size_type> element) {
               container.push_back(element);
             }
  [[nodiscard]] std::optional<C> get(size_type k) const {
    if (k >= size()) {
      return std::nullopt;
    }

    C sequence;
    size_type current = 0;
    while (true) {
      if (k < nodes_[current].terminalCount) {
        return sequence;
      }
      k -= nodes_[current].terminalCount;

      for (size_type index = 0; index < AlphabetSize; ++index) {
        const size_type child = nodes_[current].next[index];
        if (child == Null || nodes_[child].prefixCount == 0) {
          continue;
        }
        if (k < nodes_[child].prefixCount) {
          sequence.push_back(std::invoke(indexToElement_, index));
          current = child;
          break;
        }
        k -= nodes_[child].prefixCount;
      }
    }
  }

  template <std::ranges::input_range R>
    requires std::invocable<const F &, std::ranges::range_reference_t<R>> &&
             std::convertible_to<
                 std::invoke_result_t<const F &,
                                      std::ranges::range_reference_t<R>>,
                 size_type>
  [[nodiscard]] size_type longestCommonPrefix(const R &sequence) const {
    size_type length = 0;
    size_type current = 0;
    for (auto &&element : sequence) {
      const size_type index =
          static_cast<size_type>(std::invoke(characterToIndex_, element));
      if (index >= AlphabetSize) {
        break;
      }
      const size_type child = nodes_[current].next[index];
      if (child == Null || nodes_[child].prefixCount == 0) {
        break;
      }
      current = child;
      ++length;
    }
    return length;
  }

  [[nodiscard]] const Node &getNode(size_type index) const {
    assert(index < nodes_.size());
    return nodes_[index];
  }

  [[nodiscard]] const Node &operator[](size_type index) const {
    return getNode(index);
  }

  void clear() {
    nodes_.clear();
    nodes_.emplace_back();
    wordCount_ = 0;
  }

  [[nodiscard]] size_type size() const noexcept {
    return wordCount_;
  }

  [[nodiscard]] bool empty() const noexcept {
    return wordCount_ == 0;
  }

  [[nodiscard]] size_type nodeCount() const noexcept {
    return nodes_.size();
  }

private:
  template <std::ranges::input_range R>
  [[nodiscard]] std::pair<size_type, size_type>
  lexicographicalCounts(const R &sequence) const {
    size_type less = 0;
    size_type current = 0;
    for (auto &&element : sequence) {
      less += nodes_[current].terminalCount;
      const size_type index =
          static_cast<size_type>(std::invoke(characterToIndex_, element));
      assert(index < AlphabetSize);
      for (size_type smaller = 0; smaller < index; ++smaller) {
        const size_type child = nodes_[current].next[smaller];
        if (child != Null) {
          less += nodes_[child].prefixCount;
        }
      }
      if (nodes_[current].next[index] == Null) {
        return {less, less};
      }
      current = nodes_[current].next[index];
    }
    return {less, less + nodes_[current].terminalCount};
  }

  [[nodiscard]] size_type createNode() {
    nodes_.emplace_back();
    return nodes_.size() - 1;
  }

  std::vector<Node> nodes_;
  size_type wordCount_ = 0;
  [[no_unique_address]] F characterToIndex_;
  [[no_unique_address]] G indexToElement_;
};
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

各nodeはrootからの列をprefixとして表す。`terminalCount`はそのnodeで終わる列の登録数、`prefixCount`はそのprefixを持つ列の登録数となる。rootの`prefixCount`は`size()`と等しい。

`insert`と`erase`はmultiplicityを扱う。`erase`は存在する数まで削除し、実際に削除した数を返す。空列はrootで終わる列として扱う。辞書順の比較には`F`が返すalphabet indexの順序を使い、重複して登録した列はその個数だけ順位に含める。`G`はalphabet indexを列の要素へ戻す逆mapとして`get`で使う。

- `countLess(sequence)`は`sequence`より辞書順で小さい登録列の個数を返す
- `countLessEqual(sequence)`は`sequence`以下の登録列の個数を返す
- `get<C>(k)`は0-indexedで小さい方から$k$番目の登録列をcontainer型`C`で返す。範囲外では`std::nullopt`を返す
- `longestCommonPrefix(sequence)`は`sequence`と登録列のいずれかとの最長共通prefix長を返す

Trieは辺をalphabet indexとして保持し、構築時に渡した`G indexToElement`で各indexを要素へ復元する。`C`はdefault constructionと、逆mapの結果に対する`push_back`を提供する必要がある。

## Requirements

- `F`は列の要素を$[0,\mathrm{AlphabetSize})$のindexへ写す
- `G`は$[0,\mathrm{AlphabetSize})$のindexを列の要素へ写す
- `insert`へ範囲外の要素を渡すとassertion failureになる
- `erase`は同じ列を確認後に再走査するためforward rangeを要求する
- `AsciiCharacterToIndex<B>`と`AsciiIndexToCharacter<B>`はASCII文字`B`から始まる連続した文字とindexを相互変換する

## Complexity
| Operation | Time | Additional space |
| --- | --- | --- |
| `insert` | $O(LA)$ worst case | $O(LA)$ |
| `erase` | $O(L)$ | $O(1)$ |
| `findNode`, `count`, `contains` | $O(L)$ | $O(1)$ |
| `countPrefix`, `containsPrefix` | $O(L)$ | $O(1)$ |
| `countLess`, `countLessEqual` | $O(LA)$ | $O(1)$ |
| `get` | $O(LA)$ | $O(L)$ |
| `longestCommonPrefix` | $O(L)$ | $O(1)$ |
| `clear` | $O(M+A)$ | $O(A)$ |

$L$は入力列の長さ、$A$は`AlphabetSize`、$M$は生成済みnode数。$A$を定数とみなせば各操作は$O(L)$となる。Trie全体は$O(MA)$の空間を使う。

## Usage
- lowercase英字には既定の`Trie<>`を使う
- uppercase英字には`Trie<26, AsciiCharacterToIndex<'A'>, AsciiIndexToCharacter<'A'>>`を使う
- 独自alphabetにはindexを返す`F`とindexから要素を復元する`G`を指定する
- `erase`はcountを減らすだけでnodeを解放しない。不要nodeをまとめて破棄する場合は`clear`を使う
- `findNode`が返すindexは`getNode`または`operator[]`へ渡せる。存在しない場合は`Null`を返す
cpsc:text:end */
