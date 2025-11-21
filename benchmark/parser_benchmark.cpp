//
// Created by Chen, WenTao on 2025/11/4.
//

#include <benchmark/benchmark.h>
#include <fmt/format.h>

#include <random>
#include <string>

#include "parser.h"

// benchmark for BasicParseOnce with random generated data in runtime
static void BM_ParseOnce_Base(benchmark::State& state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> char_dist('a', 'z');
  std::uniform_int_distribution temp_int_dist(-99, 99);
  std::uniform_int_distribution temp_float_dist(0, 9);

  constexpr auto kNameLength = 100;

  std::string name;
  for (int i = 0; i < kNameLength; ++i) {
    name += static_cast<char>(char_dist(gen));
  }

  auto data = fmt::format("{};{}.{}\n", name, temp_int_dist(gen), temp_float_dist(gen));

  for (auto _ : state) {
    std::span<const char> span(data);
    benchmark::DoNotOptimize(ParseOnce_Base(span));
  }
}

template <auto Func>
static void BM_FindFirstZeroByte_Templated(benchmark::State& state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution num_dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

  std::array<uint64_t, 1000> data{};
  for (auto& val : data) {
    val = num_dist(gen);
  }

  for (auto _ : state) {
    for (const auto num : data) {
      benchmark::DoNotOptimize(Func(num));
    }
  }
}

template <auto Func>
static void BM_ParseNumber_Templated(benchmark::State& state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution negative_dist(0, 1);
  std::uniform_int_distribution digit_dist(0, 999);

  constexpr size_t kInputSize = 1e6;
  std::vector<std::string> inputs;
  for (auto i = 0; i < kInputSize; ++i) {
    const auto n = digit_dist(gen);
    inputs.push_back(fmt::format("{}{}.{}\n", negative_dist(gen) ? "-" : "", n / 10, n % 10));
  }

  size_t index = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(Func(inputs[index++]));
    index %= kInputSize;
  }
}

BENCHMARK(BM_ParseOnce_Base)->Repetitions(5)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_FindFirstZeroByte_Templated, FindFirstZeroByte_Base)->Repetitions(10)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_FindFirstZeroByte_Templated, FindFirstZeroByte_SWAR)->Repetitions(10)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ParseNumber_Templated, ParseNumber_Base)->Repetitions(10)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ParseNumber_Templated, ParseNumber_SWAR)->Repetitions(10)->DisplayAggregatesOnly();
BENCHMARK_TEMPLATE(BM_ParseNumber_Templated, ParseNumber_V3)->Repetitions(10)->DisplayAggregatesOnly();
