//
// Created by Chen, WenTao on 2025/9/7.
//

#include "parser/parse_station.h"

#include <benchmark/benchmark.h>

#include <print>

#include "utils/env.h"
#include "utils/file_memory_map.h"

static void BM_ParseStation_V1(benchmark::State &state) {
  auto &env = Env::Instance();
  const FileMemoryMap memory_map(env.input_file);
  const auto span = static_cast<std::span<const char>>(memory_map);
  auto ptr = span.data();
  for (auto _ : state) {
    auto [name, temperature] = ParseStation_V1(ptr);
    benchmark::DoNotOptimize(name);
    benchmark::DoNotOptimize(temperature);
  }
}

static void BM_ParseStation_V2(benchmark::State &state) {
  auto &env = Env::Instance();
  const FileMemoryMap memory_map(env.input_file);
  const auto span = static_cast<std::span<const char>>(memory_map);
  auto ptr = span.data();
  for (auto _ : state) {
    auto [name, temperature] = ParseStation_V2(ptr);
    benchmark::DoNotOptimize(name);
    benchmark::DoNotOptimize(temperature);
  }
}

static void BM_ParseStation_V3(benchmark::State &state) {
  auto &env = Env::Instance();
  const FileMemoryMap memory_map(env.input_file);
  auto span = static_cast<std::span<const char>>(memory_map);
  for (auto _ : state) {
    auto [name, temperature] = ParseStation_V3(span);
    benchmark::DoNotOptimize(name);
    benchmark::DoNotOptimize(temperature);
  }
}

BENCHMARK(BM_ParseStation_V1);
BENCHMARK(BM_ParseStation_V2);
BENCHMARK(BM_ParseStation_V3);
