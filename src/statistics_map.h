//
// Created by Chen, WenTao on 2025/12/21.
//

#pragma once

#include <array>
#include <cassert>
#include <string_view>
#include <unordered_map>

#include "data.h"

template <>
struct std::hash<StationName> {
  size_t operator()(const StationName& name) const noexcept { return name.hash; }
};

// using StatisticsMap = std::unordered_map<StationName, Statistics>;

struct StatisticsMap {
  constexpr static size_t Size = 1 << 16;
  using Key = StationName;
  struct KV {
    Key key{};
    Statistics value{};
  };
  struct KeyValueStore {
    bool used{};
    KV kv{};
  };
  using Index = size_t;
  using Bucket = std::array<KeyValueStore, Size>;

  Statistics& operator[](const Key& key) {
    const auto hash = std::hash<Key>{}(key);
    const auto index = hash % Size;
    assert(index < Size);

    size_t pos = index;
    for (size_t i = 0; i < Size && bucket[pos].used; pos = (index + ++i) % Size) {
      if (bucket[pos].kv.key == key) {
        return bucket[pos].kv.value;
      }
    }

    if (!bucket[pos].used) {
      bucket[pos] = KeyValueStore{true, {key, {}}};
      return bucket[pos].kv.value;
    }
    assert(false);
  }

  struct Iterator {
    using iterator_concept = std::forward_iterator_tag;
    using value_type = KV;
    using difference_type = std::ptrdiff_t;
    using pointer = KV*;
    using reference = KV&;

    const KeyValueStore* curr;
    const KeyValueStore* end;

    void skip_unused() {
      while (curr != end && !curr->used) {
        ++curr;
      }
    }

    reference operator*() const { return const_cast<reference>(curr->kv); }
    pointer operator->() const { return const_cast<pointer>(&curr->kv); }

    Iterator& operator++() {
      ++curr;
      skip_unused();
      return *this;
    }

    Iterator operator++(int) {
      const auto tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const Iterator&) const = default;
  };

  [[nodiscard]] auto begin() const {
    Iterator it{bucket.data(), bucket.data() + Size};
    it.skip_unused();
    return it;
  }

  [[nodiscard]] auto end() const { return Iterator{bucket.data() + Size, bucket.data() + Size}; }

  Bucket bucket;
};

static_assert(std::forward_iterator<StatisticsMap::Iterator>);