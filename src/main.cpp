//
// Created by Chen, WenTao on 2025/11/2.
//

#include <fmt/format.h>

#include <algorithm>

#include "env.h"
#include "file_memory_map.h"
#include "parallelizer.h"

int main() {
  const FileMemoryMap file(Env::Instance().input_file);
  Parallelizer parallelizer(Env::Instance().threads, 1 << 21, static_cast<std::span<const char>>(file));
  const auto result = parallelizer.Process();

  auto v = result | std::ranges::to<std::vector<StatisticsMap::KV>>();
  std::ranges::sort(v, {}, &StatisticsMap::KV::key);

  for (const auto& [key, statistics] : v) {
    fmt::print("{}={:.1f}/{:.1f}/{:.1f} ", key.name, statistics.min / 10.0,
               static_cast<double>(statistics.sum) / statistics.count / 10.0, statistics.max / 10.0);
  }

  return 0;
}
