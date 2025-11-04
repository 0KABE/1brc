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
  std::uniform_int_distribution char_dist('a', 'z');
  std::uniform_int_distribution temp_int_dist(-99, 99);
  std::uniform_int_distribution temp_float_dist(0, 9);

  constexpr auto kNameLength = 100;

  std::string name;
  for (int i = 0; i < kNameLength; ++i) {
    name += char_dist(gen);
  }

  auto data = fmt::format("{};{}.{}\n", name, temp_int_dist(gen), temp_float_dist(gen));
  std::span<const char> span(data);

  for (auto _ : state) {
    benchmark::DoNotOptimize(ParseOnce_Base(span));
  }
}

BENCHMARK(BM_ParseOnce_Base)->Repetitions(5);
