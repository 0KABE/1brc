//
// Created by Chen, WenTao on 2025/11/4.
//

#include <benchmark/benchmark.h>

#include <random>
#include <string>
#include <vector>

#include "parser.h"

// benchmark for BasicParseOnce with random generated data in runtime
static void BM_BasicParseOnce(benchmark::State& state) {
  std::string data;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution name_len_dist(1, 100);
  std::uniform_int_distribution char_dist('a', 'z');
  std::uniform_int_distribution temp_int_dist(-99, 99);
  std::uniform_int_distribution temp_float_dist(0, 9);

  for (int i = 0; i < 10; ++i) {
    const int name_len = name_len_dist(gen);
    for (int j = 0; j < name_len; ++j) {
      data += char_dist(gen);
    }
    data += ';';
    data += std::to_string(temp_int_dist(gen));
    data += '.';
    data += std::to_string(temp_float_dist(gen));
    data += '\n';
  }

  for (auto _ : state) {
    std::span<const char> span(data);
    while (!span.empty()) {
      benchmark::DoNotOptimize(BasicParseOnce(span));
    }
  }
}

BENCHMARK(BM_BasicParseOnce);
