//
// Created by Chen, WenTao on 2025/8/19.
//

#include "parse_number.h"
#include "utils/env.h"
#include "utils/file_memory_map.h"

#include <benchmark/benchmark.h>
#include <format>
#include <print>

static void BM_ParseNumber_V1(benchmark::State &state) {
  std::array<std::string, 2> data = {"-12.9xxxxxx", "12.9xxxxxxxx"};
  size_t index = 0;
  for (auto _ : state) {
    auto n = parse_number_v1(data[index++]);
    index = index % data.size();
    benchmark::DoNotOptimize(n);
  }
}

static void BM_ParseNumber_V2(benchmark::State &state) {
  std::array<std::string, 2> data = {"-12.9xxxxxx", "12.9xxxxxxxx"};
  size_t index = 0;
  for (auto _ : state) {
    auto n = parse_number_v2(data[index++]);
    index = index % data.size();
    benchmark::DoNotOptimize(n);
  }
}

BENCHMARK(BM_ParseNumber_V1);
BENCHMARK(BM_ParseNumber_V2);
