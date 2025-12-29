//
// Created by Chen, WenTao on 2025/12/23.
//

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include "benchmark_util.h"
#include "hash.h"

template <auto Func>
static void BM_Hash(benchmark::State& state) {
  constexpr int kInputSize = 1 << 10;
  std::vector<std::string> inputs;
  inputs.reserve(kInputSize);
  for (auto i = 0; i < kInputSize; ++i) {
    inputs.push_back(fmt::format("{}", RandomStation()));
  }

  int index = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(Func(inputs[index++]));
    index %= kInputSize;
  }
}

BENCHMARK_TEMPLATE(BM_Hash, std::hash<std::string_view>{})->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_Hash, FastHash)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_Hash, SimpleHash)->DisplayAggregatesOnly();
