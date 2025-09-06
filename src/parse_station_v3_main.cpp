//
// Created by Chen, WenTao on 2025/8/19.
//

#include <benchmark/benchmark.h>

#include "parser/parse_station.h"
#include "utils/env.h"
#include "utils/file_memory_map.h"

int main() {
  const auto &env = Env::Instance();
  const FileMemoryMap memory_map(env.input_file);
  auto span = static_cast<std::span<const char>>(memory_map);

  while (!span.empty()) {
    auto [name, temperature] = ParseStation_V3(span);
    benchmark::DoNotOptimize(name);
    benchmark::DoNotOptimize(temperature);
  }
}
