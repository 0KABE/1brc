//
// Created by Chen, WenTao on 2025/9/7.
//

#pragma once

#include <cassert>
#include <string_view>
#include <thread>

#include "structs.h"
#include "utils/file_memory_map.h"

struct HashMap {
  constexpr static size_t Size = 81920;
  using Key = std::string_view;
  struct Value {
    bool used{};
    Key key;
    StationStatistics value{};
  };
  using Index = size_t;
  using Bucket = std::array<Value, Size>;

  StationStatistics &operator[](const Key &key) {
    const auto hash = std::hash<Key>{}(key);
    const auto index = hash % Size;
    assert(index < Size);

    size_t pos = index;
    for (size_t i = 0; i < Size && bucket[pos].used; pos = (index + ++i) % Size) {
      if (bucket[pos].key == key) {
        return bucket[pos].value;
      }
    }

    if (!bucket[pos].used) {
      bucket[pos] = Value{true, key};
      return bucket[pos].value;
    }
    assert(false);
  }

  Bucket bucket;
};

void Merge(Statistics &dest, HashMap::Bucket &bucket) {
  static std::mutex mutex;
  std::lock_guard lock(mutex);
  for (auto &[used, key, value] : bucket) {
    if (!used) {
      continue;
    }

    if (const auto it = dest.find(key); it != dest.end()) {
      it->second.count += value.count;
      it->second.min = std::min(it->second.min, value.min);
      it->second.max = std::max(it->second.max, value.max);
      it->second.sum += value.sum;
    } else {
      dest[key] = value;
    }
  }
}
