//
// Created by Chen, WenTao on 2025/11/4.
//

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include <random>
#include <string>

#include "single_line_reader.h"

std::string RandomStation() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution name_length_dist(1, 100);
  std::uniform_int_distribution<> char_dist('a', 'z');

  std::string name;
  const auto length = name_length_dist(gen);
  for (int i = 0; i < length; ++i) {
    name += static_cast<char>(char_dist(gen));
  }
  return name;
}

std::string RandomTemperature() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution negative_dist(0, 1);
  std::uniform_int_distribution digit_dist(0, 999);

  const auto n = digit_dist(gen);
  return fmt::format("{}{}.{}\n", negative_dist(gen) ? "-" : "", n / 10, n % 10);
}

template <SingleLineReadable Func>
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

template <auto Func>
static void BM_FindFirstZeroByte_Templated(benchmark::State& state) {
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
      benchmark::DoNotOptimize(Func(num));
    }
  }
}

template <NumberReadable Func>
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

BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader_Base)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_Base, NumberReader_Base>)
    ->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_Base, NumberReader_SWAR_V1>)
    ->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_Base, NumberReader_SWAR_V2>)
    ->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_SWAR, NumberReader_Base>)
    ->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_SWAR, NumberReader_SWAR_V1>)
    ->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadSingleLine, SingleLineReader<FindFirstZeroByte_SWAR, NumberReader_SWAR_V2>)
    ->DisplayAggregatesOnly();

BENCHMARK_TEMPLATE(BM_FindFirstZeroByte_Templated, FindFirstZeroByte_Base)->Arg(1 << 4)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_FindFirstZeroByte_Templated, FindFirstZeroByte_SWAR)->Arg(1 << 4)->DisplayAggregatesOnly();

BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_Base)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_SWAR_V1)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ReadNumber, NumberReader_SWAR_V2)->DisplayAggregatesOnly();
