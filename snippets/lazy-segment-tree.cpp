/* cpsc:meta:start
slug: lazy-segment-tree
title: Lazy Segment Tree
shortTitle: Lazy Segment Tree
category: Data Structure
description: モノイドの区間積と、区間全体への作用を遅延伝播によって対数時間で処理する。
language:
  name: C++
  version: "20"
  prism: cpp
cpsc:meta:end */

/* cpsc:text:start
## Requirements
値の集合を $S$、作用素の集合を $F$ とする。この実装が正しく動作するには、次の条件をすべて満たす必要がある。

### 1. 値のモノイド

`op` と `unit` はモノイド $(S,\operatorname{op},\mathrm{unit})$ をなす。

$$
\begin{aligned}
\operatorname{op}&:S\times S\to S,
&\mathrm{unit}&\in S \\
\operatorname{op}(\operatorname{op}(a,b),c)
  &= \operatorname{op}(a,\operatorname{op}(b,c))
&& (a,b,c\in S) \\
\operatorname{op}(\mathrm{unit},a)
  &= \operatorname{op}(a,\mathrm{unit})=a
&& (a\in S)
\end{aligned}
$$

### 2. 作用素のモノイド

`composition` と `identity` はモノイド $(F,\operatorname{composition},\mathrm{identity})$ をなす。`composition(f, g)` は $g$ の後に $f$ を適用する作用を表す。

$$
\begin{aligned}
\operatorname{composition}&:F\times F\to F,
&\mathrm{identity}&\in F \\
\operatorname{composition}(f,\operatorname{composition}(g,h))
  &= \operatorname{composition}(\operatorname{composition}(f,g),h)
&& (f,g,h\in F) \\
\operatorname{composition}(\mathrm{identity},f)
  &= \operatorname{composition}(f,\mathrm{identity})=f
&& (f\in F)
\end{aligned}
$$

### 3. 自己準同型による左モノイド作用

`mapping` は $F\times S\to S$ であり、$S$ をleft $F$-actにする。恒等作用を保ち、作用の合成と整合する。

$$
\begin{aligned}
\operatorname{mapping}(\mathrm{identity},a)&=a
&& (a\in S) \\
\operatorname{mapping}(\operatorname{composition}(f,g),a)
  &=\operatorname{mapping}(f,\operatorname{mapping}(g,a))
&& (f,g\in F,\ a\in S)
\end{aligned}
$$

さらに各 $f\in F$ が誘導する写像 $\operatorname{mapping}(f,-):S\to S$ は、値のモノイドの自己準同型となる。つまり、単位元と演算を保存する。

$$
\begin{aligned}
\operatorname{mapping}(f,\mathrm{unit})&=\mathrm{unit}
&& (f\in F) \\
\operatorname{mapping}(f,\operatorname{op}(a,b))
  &=\operatorname{op}(\operatorname{mapping}(f,a),
                      \operatorname{mapping}(f,b))
&& (f\in F,\ a,b\in S)
\end{aligned}
$$

左モノイド作用の条件によって遅延作用を合成でき、自己準同型の条件によって作用を各要素へ配らず区間積へ直接適用できる。

## Complexity
| Operation | Complexity |
| --- | --- |
| Construction | $O(N)$ |
| `set` / `get` / `product` / `apply` / `partitionPoint` / `partitionPointReverse` | $O(\log N)$ |
| `allProduct` / `size` / `empty` | $O(1)$ |
| `clear` | $O(N)$ |
| Space | $O(N)$ |

## Usage
- `mapping(f, value)` は作用 $f$ を区間の集約値へ適用する
- 区間和へ区間加算を載せる場合、値に区間長も保持し、`mapping` で加算量と区間長の積を加える
- `product(left, right)` と区間版 `apply` は半開区間 $[left,right)$ を扱う
- `partitionPoint` のpredicateは単位元に対してtrueとなり、積を延長するとtrueからfalseへ単調に変化するものとする
- 範囲外のindexや不正な区間を渡すとassertionに失敗する
cpsc:text:end */

// cpsc:subsnippet:start LazySegmentTree
template <typename T, typename Op, typename F, typename Mapping,
          typename Composition>
class LazySegmentTree {
public:
  using value_type = T;
  using operator_type = F;
  using size_type = std::size_t;

private:
  size_type length_;
  size_type leafCount_;
  size_type height_;
  std::vector<value_type> tree_;
  std::vector<operator_type> lazy_;
  [[no_unique_address]] Op op_;
  [[no_unique_address]] Mapping mapping_;
  [[no_unique_address]] Composition composition_;
  value_type unit_;
  operator_type identity_;

  void update(size_type index) {
    tree_[index] = op_(tree_[2 * index], tree_[2 * index + 1]);
  }

  void allApply(size_type index, const operator_type &f) {
    tree_[index] = mapping_(f, tree_[index]);
    if (index < leafCount_) {
      lazy_[index] = composition_(f, lazy_[index]);
    }
  }

  void push(size_type index) {
    allApply(2 * index, lazy_[index]);
    allApply(2 * index + 1, lazy_[index]);
    lazy_[index] = identity_;
  }

public:
  LazySegmentTree()
      : LazySegmentTree(0, Op{}, Mapping{}, Composition{}, value_type{},
                        operator_type{}) {}

  explicit LazySegmentTree(size_type n, Op op, Mapping mapping,
                           Composition composition, value_type unit,
                           operator_type identity)
      : LazySegmentTree(std::vector<value_type>(n, unit), op, mapping,
                        composition, unit, identity) {}

  explicit LazySegmentTree(const std::vector<value_type> &v, Op op,
                           Mapping mapping, Composition composition,
                           value_type unit, operator_type identity)
      : length_(v.size()), leafCount_(std::bit_ceil(length_)),
        height_(static_cast<size_type>(std::countr_zero(leafCount_))),
        tree_(2 * leafCount_, unit), lazy_(leafCount_, identity), op_(op),
        mapping_(mapping), composition_(composition), unit_(unit),
        identity_(identity) {
    for (size_type i = 0; i < length_; ++i) {
      tree_[leafCount_ + i] = v[i];
    }
    for (size_type i = leafCount_ - 1; i >= 1; --i) {
      update(i);
    }
  }

  void set(size_type index, const value_type &value) {
    assert(index < length_);
    index += leafCount_;
    for (size_type i = height_; i >= 1; --i) {
      push(index >> i);
    }
    tree_[index] = value;
    for (size_type i = 1; i <= height_; ++i) {
      update(index >> i);
    }
  }

  [[nodiscard]] value_type get(size_type index) {
    assert(index < length_);
    index += leafCount_;
    for (size_type i = height_; i >= 1; --i) {
      push(index >> i);
    }
    return tree_[index];
  }

  [[nodiscard]] value_type operator[](size_type index) {
    return get(index);
  }

  [[nodiscard]] value_type product(size_type left, size_type right) {
    assert(left <= right && right <= length_);
    if (left == right) {
      return unit_;
    }

    left += leafCount_;
    right += leafCount_;

    for (size_type i = height_; i >= 1; --i) {
      if (((left >> i) << i) != left) {
        push(left >> i);
      }
      if (((right >> i) << i) != right) {
        push((right - 1) >> i);
      }
    }

    value_type leftProduct = unit_;
    value_type rightProduct = unit_;
    while (left < right) {
      if (left & 1) {
        leftProduct = op_(leftProduct, tree_[left++]);
      }
      if (right & 1) {
        rightProduct = op_(tree_[--right], rightProduct);
      }
      left >>= 1;
      right >>= 1;
    }
    return op_(leftProduct, rightProduct);
  }

  [[nodiscard]] value_type allProduct() const {
    return tree_[1];
  }

  void apply(size_type index, const operator_type &f) {
    assert(index < length_);
    index += leafCount_;
    for (size_type i = height_; i >= 1; --i) {
      push(index >> i);
    }
    tree_[index] = mapping_(f, tree_[index]);
    for (size_type i = 1; i <= height_; ++i) {
      update(index >> i);
    }
  }

  void apply(size_type left, size_type right, const operator_type &f) {
    assert(left <= right && right <= length_);
    if (left == right) {
      return;
    }

    left += leafCount_;
    right += leafCount_;

    for (size_type i = height_; i >= 1; --i) {
      if (((left >> i) << i) != left) {
        push(left >> i);
      }
      if (((right >> i) << i) != right) {
        push((right - 1) >> i);
      }
    }

    size_type l = left;
    size_type r = right;
    while (l < r) {
      if (l & 1) {
        allApply(l++, f);
      }
      if (r & 1) {
        allApply(--r, f);
      }
      l >>= 1;
      r >>= 1;
    }

    for (size_type i = 1; i <= height_; ++i) {
      if (((left >> i) << i) != left) {
        update(left >> i);
      }
      if (((right >> i) << i) != right) {
        update((right - 1) >> i);
      }
    }
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, size_type right,
                                         P predicate)
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));
    if (left == right) {
      return left;
    }

    left += leafCount_;
    for (size_type i = height_; i >= 1; --i) {
      if (((left >> i) << i) != left) {
        push(left >> i);
      }
    }

    value_type currentProduct = unit_;
    do {
      while ((left & 1) == 0) {
        left >>= 1;
      }
      if (!predicate(op_(currentProduct, tree_[left]))) {
        while (left < leafCount_) {
          push(left);
          left = 2 * left;
          if (predicate(op_(currentProduct, tree_[left]))) {
            currentProduct = op_(currentProduct, tree_[left]);
            left++;
          }
        }
        return std::min(left - leafCount_, right);
      }
      currentProduct = op_(currentProduct, tree_[left]);
      left++;
    } while ((left & -left) != left);

    return right;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPoint(size_type left, P predicate)
    requires std::predicate<P, value_type> {
    return partitionPoint(left, length_, predicate);
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type left, size_type right,
                                                P predicate)
    requires std::predicate<P, value_type> {
    assert(left <= right && right <= length_);
    assert(predicate(unit_));
    if (left == right) {
      return right;
    }

    right += leafCount_;
    for (size_type i = height_; i >= 1; --i) {
      if ((((right - 1) >> i) << i) != (right - 1)) {
        push((right - 1) >> i);
      }
    }

    value_type currentProduct = unit_;
    do {
      right--;
      while (right > 1 && (right & 1)) {
        right >>= 1;
      }
      if (!predicate(op_(tree_[right], currentProduct))) {
        while (right < leafCount_) {
          push(right);
          right = 2 * right + 1;
          if (predicate(op_(tree_[right], currentProduct))) {
            currentProduct = op_(tree_[right], currentProduct);
            right--;
          }
        }
        return std::max(right + 1 - leafCount_, left);
      }
      currentProduct = op_(tree_[right], currentProduct);
    } while ((right & -right) != right);

    return left;
  }

  template <typename P>
  [[nodiscard]] size_type partitionPointReverse(size_type right, P predicate)
    requires std::predicate<P, value_type> {
    return partitionPointReverse(0, right, predicate);
  }

  void clear() {
    std::fill(tree_.begin(), tree_.end(), unit_);
    std::fill(lazy_.begin(), lazy_.end(), identity_);
  }

  [[nodiscard]] size_type size() const noexcept {
    return length_;
  }
  [[nodiscard]] bool empty() const noexcept {
    return length_ == 0;
  }
};
// cpsc:subsnippet:end
