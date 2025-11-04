#include <benchmark/benchmark.h>

// Templated benchmark for addition
template <typename T>
static void BM_Addition(benchmark::State& state) {
  struct Entity {
    std::string_view name;
    T temperature;
  };
  Entity entity{};
  for (auto _ : state) {
    benchmark::DoNotOptimize(entity.temperature += 3);
  }
}

// Templated benchmark for timing
template <typename T>
static void BM_Timing(benchmark::State& state) {
  struct Entity {
    std::string_view name;
    T temperature;
  };
  Entity entity{.temperature = 3};
  for (auto _ : state) {
    benchmark::DoNotOptimize(entity.temperature *= 3);
  }
}

BENCHMARK_TEMPLATE(BM_Addition, char);
BENCHMARK_TEMPLATE(BM_Addition, short);
BENCHMARK_TEMPLATE(BM_Addition, int);
BENCHMARK_TEMPLATE(BM_Addition, long);
BENCHMARK_TEMPLATE(BM_Addition, long long);
BENCHMARK_TEMPLATE(BM_Addition, unsigned char);
BENCHMARK_TEMPLATE(BM_Addition, unsigned short);
BENCHMARK_TEMPLATE(BM_Addition, unsigned int);
BENCHMARK_TEMPLATE(BM_Addition, unsigned long);
BENCHMARK_TEMPLATE(BM_Addition, unsigned long long);

BENCHMARK_TEMPLATE(BM_Timing, char);
BENCHMARK_TEMPLATE(BM_Timing, short);
BENCHMARK_TEMPLATE(BM_Timing, int);
BENCHMARK_TEMPLATE(BM_Timing, long);
BENCHMARK_TEMPLATE(BM_Timing, long long);
BENCHMARK_TEMPLATE(BM_Timing, unsigned char);
BENCHMARK_TEMPLATE(BM_Timing, unsigned short);
BENCHMARK_TEMPLATE(BM_Timing, unsigned int);
BENCHMARK_TEMPLATE(BM_Timing, unsigned long);
BENCHMARK_TEMPLATE(BM_Timing, unsigned long long);
