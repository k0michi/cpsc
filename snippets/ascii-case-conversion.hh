/* cpsc:meta:start
slug: ascii-case-conversion
title: ASCII Case Conversion
shortTitle: Case Conversion
category: String
description: ASCII英字の大文字と小文字をlocaleに依存せず変換する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

#pragma once

// cpsc:subsnippet:start toLowerCase / toUpperCase
[[nodiscard]] constexpr char toLowerCase(char character) noexcept {
  if ('A' <= character && character <= 'Z') {
    return static_cast<char>(character + ('a' - 'A'));
  }
  return character;
}

[[nodiscard]] constexpr char toUpperCase(char character) noexcept {
  if ('a' <= character && character <= 'z') {
    return static_cast<char>(character - ('a' - 'A'));
  }
  return character;
}
[[nodiscard]] constexpr std::string toLowerCase(std::string_view string) {
  std::string result(string);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](char character) { return toLowerCase(character); });
  return result;
}

[[nodiscard]] constexpr std::string toUpperCase(std::string_view string) {
  std::string result(string);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](char character) { return toUpperCase(character); });
  return result;
}

constexpr void toLowerCaseInPlace(std::string &string) noexcept {
  std::transform(string.begin(), string.end(), string.begin(),
                 [](char character) { return toLowerCase(character); });
}

constexpr void toUpperCaseInPlace(std::string &string) noexcept {
  std::transform(string.begin(), string.end(), string.begin(),
                 [](char character) { return toUpperCase(character); });
}
// cpsc:subsnippet:end

/* cpsc:text:start
## Semantics

`toLowerCase`はASCII大文字を小文字へ、`toUpperCase`はASCII小文字を大文字へ変換する。ASCII英字以外のbyteは変更しない。

`char` overloadは1文字を返す。`string_view` overloadは変換済みの新しい`string`を返す。`toLowerCaseInPlace`と`toUpperCaseInPlace`は渡された`string`自体を変換する。

## Complexity
| Operation | Time | Space |
| --- | --- | --- |
| Character | $O(1)$ | $O(1)$ |
| Copy | $O(N)$ | $O(N)$ |
| In-place conversion | $O(N)$ | $O(1)$ |

## Usage
- ASCIIだけを扱う問題向けで、localeやUnicodeの大小文字変換には使用しない
- `std::tolower`や`std::toupper`と異なり、負の値を持つ`char`を渡しても未定義動作にならず、そのまま返す
cpsc:text:end */
