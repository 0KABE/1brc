//
// Created by Chen, WenTao on 2025/12/19.
//

#include <benchmark/benchmark.h>

#include "benchmark_util.h"
#include "single_line_reader.h"

template <SingleLineReaderFunctor Func>
static void BM_ReadSingleLine(benchmark::State& state) {
  constexpr int kInputSize = 1 << 20;
  std::vector<std::string> inputs;
  inputs.reserve(kInputSize);
  for (auto i = 0; i < kInputSize; ++i) {
    inputs.push_back(fmt::format("{};{}\n", RandomStation(), RandomTemperature()));
  }

  int index = 0;
  for (auto _ : state) {
    std::span<const char> span = inputs[index++];
    benchmark::DoNotOptimize(Func{}(span));
    index %= kInputSize;
  }
}

BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_Base)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V1)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V2)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V3)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V4)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V5)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V6)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V7)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V8)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_V9)->DisplayAggregatesOnly();
