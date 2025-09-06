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
  const auto span = static_cast<std::span<const char>>(memory_map);
  auto ptr = span.data();

  while (ptr < span.data() + span.size()) {
    auto [name, temperature] = ParseStation_V2(ptr);
    benchmark::DoNotOptimize(name);
    benchmark::DoNotOptimize(temperature);
  }
}
