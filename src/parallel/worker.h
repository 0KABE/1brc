//
// Created by Chen, WenTao on 2025/9/7.
//

#pragma once

#include <span>
#include <thread>

#include "../parser/parse_station.h"
#include "../parser/structs.h"
#include "utils/file_memory_map.h"
#include "utils/hash_map.h"

struct Worker {
  unsigned index;
  const FileMemoryMap &file;
  std::span<char> span;
  Statistics &all_statistics;
  std::thread thread;

  void Run() {
    HashMap statistics;
    auto [begin, end] = AlignSpan();
    while (begin < end) {
      auto [name, temperature] = ParseStation_V2(begin);

      auto &[count, min, max, sum] = statistics[name];
      max = std::max(max, temperature);
      min = std::min(min, temperature);
      sum += temperature;
      count++;
    }
    Merge(all_statistics, statistics.bucket);
  }

  [[nodiscard]] std::tuple<const char *, const char *> AlignSpan() const {
    const char *begin = &span.front();
    const char *end = &span.back();
    const char *file_end = static_cast<const char *>(file.address) + file.size;

    while (end < file_end && *end != '\n') {
      ++end;
    }
    while (begin < end && *begin != '\n') {
      ++begin;
    }
    ++begin;

    return {begin, end};
  }
};
