//
// Created by Chen, WenTao on 2025/11/4.
//

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include <random>
#include <string>

#include "benchmark_util.h"
#include "single_line_reader.h"

template <FindLowestZeroByteFunctor Func>
static void BM_FindFirstZeroByte(benchmark::State& state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution num_dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

  const size_t kInputSize = state.range(0);
  std::vector<uint64_t> data(kInputSize);
  for (auto& val : data) {
    val = num_dist(gen);
  }

  for (auto _ : state) {
    for (auto num : data) {
      benchmark::DoNotOptimize(Func{}(num));
    }
  }
}

template <NumberReaderFunctor Func>
static void BM_ReadNumber(benchmark::State& state) {
  constexpr int kInputSize = 1 << 20;
  std::vector<std::string> inputs;
  inputs.reserve(kInputSize);
  for (auto i = 0; i < kInputSize; ++i) {
    inputs.push_back(RandomTemperature());
  }

  int index = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(Func{}(inputs[index++]));
    index %= kInputSize;
  }
}

BENCHMARK_TEMPLATE(BM_FindFirstZeroByte, FindLowestZeroByte_Base)->Arg(1 << 4)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_FindFirstZeroByte, FindLowestZeroByte_SWAR)->Arg(1 << 4)->DisplayAggregatesOnly();

BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_Base)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_SWAR_V1)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_SWAR_V2)->DisplayAggregatesOnly();
