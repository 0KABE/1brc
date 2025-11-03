//
// Created by Chen, WenTao on 2025/11/2.
//

#include <benchmark/benchmark.h>

#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Function to generate a random string of a given length
std::string generate_random_string(const int length) {
  static constexpr char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  thread_local std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> distrib(0, sizeof(alphanum) - 2);

  std::string tmp_s;
  tmp_s.reserve(length);
  for (int i = 0; i < length; ++i) {
    tmp_s += alphanum[distrib(gen)];
  }

  return tmp_s;
}

constexpr int NUM_KEYS = 10000;
constexpr int STRING_LENGTH = 10;
constexpr int NUM_UNIQUE_KEYS = 100;

template <typename KeyType>
static void BM_UnorderedMap(benchmark::State& state) {
  static std::vector<KeyType> keys;
  static std::vector<std::string> string_data_for_views;

  if (keys.empty() && string_data_for_views.empty()) {
    using UniqueKeyType =
        typename std::conditional<std::is_same_v<KeyType, std::string_view>,
                                  std::string, KeyType>::type;
    std::vector<UniqueKeyType> unique_keys;
    unique_keys.reserve(NUM_UNIQUE_KEYS);

    if constexpr (std::is_same_v<KeyType, std::string> ||
                  std::is_same_v<KeyType, std::string_view>) {
      for (int i = 0; i < NUM_UNIQUE_KEYS; ++i) {
        unique_keys.push_back(generate_random_string(STRING_LENGTH));
      }
    } else if constexpr (std::is_integral_v<KeyType>) {
      thread_local std::mt19937 gen(std::random_device{}());
      std::uniform_int_distribution<KeyType> distrib;
      for (int i = 0; i < NUM_UNIQUE_KEYS; ++i) {
        unique_keys.push_back(distrib(gen));
      }
    }

    thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> distrib(0, NUM_UNIQUE_KEYS - 1);

    if constexpr (std::is_same_v<KeyType, std::string_view>) {
      string_data_for_views.reserve(NUM_KEYS);
      for (int i = 0; i < NUM_KEYS; ++i) {
        string_data_for_views.push_back(unique_keys[distrib(gen)]);
      }
      keys.reserve(NUM_KEYS);
      for (const auto& s : string_data_for_views) {
        keys.emplace_back(s);
      }
    } else {
      keys.reserve(NUM_KEYS);
      for (int i = 0; i < NUM_KEYS; ++i) {
        keys.push_back(unique_keys[distrib(gen)]);
      }
    }
  }

  for (auto _ : state) {
    std::unordered_map<KeyType, int> map;
    for (int i = 0; i < NUM_KEYS; ++i) {
      map[keys[i]] = i;
    }
    for (int i = 0; i < NUM_KEYS; ++i) {
      benchmark::DoNotOptimize(map.at(keys[i]));
    }
  }
}

BENCHMARK_TEMPLATE(BM_UnorderedMap, int16_t);
BENCHMARK_TEMPLATE(BM_UnorderedMap, int32_t);
BENCHMARK_TEMPLATE(BM_UnorderedMap, int64_t);
BENCHMARK_TEMPLATE(BM_UnorderedMap, std::string);
BENCHMARK_TEMPLATE(BM_UnorderedMap, std::string_view);

BENCHMARK_MAIN();
