//
// Created by Chen, WenTao on 2025/11/2.
//

#include <fmt/format.h>
#include <fmt/ranges.h>

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

  const auto& formated_results =
      v | std::views::transform([](const StatisticsMap::KV& kv) {
        return fmt::format("{}={:.1f}/{:.1f}/{:.1f}", kv.key.name, kv.value.min / 10.0,
                           static_cast<double>(kv.value.sum) / kv.value.count / 10.0, kv.value.max / 10.0);
      });
  fmt::print("{{{}}}", fmt::join(formated_results, ", "));

  return 0;
}
