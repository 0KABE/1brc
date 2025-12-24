//
// Created by Chen, WenTao on 2025/12/21.
//

#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <string_view>

#include "data.h"

template <>
struct std::hash<StationName> {
  size_t operator()(const StationName &name) const noexcept { return name.hash; }
};

struct StatisticsMap {
  constexpr static size_t Size = 1 << 16;
  using Key = StationName;
  struct Value {
    bool used{};
    Key key;
    Statistics value{};
  };
  using Index = size_t;
  using Bucket = std::array<Value, Size>;

  Statistics &operator[](const Key &key) {
    const auto hash = std::hash<Key>{}(key);
    const auto index = hash % Size;
    assert(index < Size);

    size_t pos = index;
    for (size_t i = 0; i < Size && bucket[pos].used; pos = (index + ++i) % Size) {
      if (bucket[pos].key == key) {
        return bucket[pos].value;
      }
      // MAP_CONFLICTS += 1;
    }

    if (!bucket[pos].used) {
      bucket[pos] = Value{true, key};
      return bucket[pos].value;
    }
    assert(false);
  }

  Bucket bucket;
};
