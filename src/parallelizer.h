//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <ranges>
#include <span>
#include <thread>
#include <unordered_map>
#include <vector>

#include "allocator.h"
#include "data.h"
#include "multi_line_reader.h"

class Parallelizer {
 public:
  Parallelizer(const int concurrency, const size_t chunk, const std::span<const char> data)
      : threads_(concurrency), chunk_size_(chunk), data_(data) {}

  StatisticsMap Process() {
    Allocator allocator(data_);
    StatisticsMap result;

    {
      std::mutex mutex;
      std::vector<std::jthread> threads;
      threads.reserve(threads_);
      for (int i = 0; i < threads_; ++i) {
        threads.emplace_back([chunk_size = chunk_size_, &allocator, &mutex, &result] {
          MultiLineReader<SingleLineReader_V6> reader;
          for (auto buff = allocator.Allocate(chunk_size); !buff.empty(); buff = allocator.Allocate(chunk_size)) {
            reader.Parse(buff);
          }

          std::lock_guard _(mutex);
          // auto all_statistics = reader.GetStatistics().bucket |
          //                       std::views::filter([](const auto& item) { return item.used; }) |
          //                       std::views::transform([](const auto& item) {
          //                         return std::tuple<StationName, Statistics>{item.key, item.value};
          //                       });
          for (const auto& [name, statistics] : reader.GetStatistics()) {
            auto& [min, max, count, sum] = result[name];
            min = std::min(min, statistics.min);
            max = std::max(max, statistics.max);
            count += statistics.count;
            sum += statistics.sum;
          }
        });
      }
    }

    return result;
  }

 private:
  int threads_{1};
  size_t chunk_size_{1 << 21};  // 10MB by default
  std::span<const char> data_;
};

class ParallelizerV2 {
 public:
  ParallelizerV2(const int concurrency, const size_t chunk, const std::span<const char> data)
      : threads_(concurrency), chunk_size_(chunk), data_(data) {}

  StatisticsMap Process() {
    Allocator allocator(data_);
    StatisticsMap result;

    {
      std::mutex mutex;
      std::vector<std::jthread> threads;
      threads.reserve(threads_);
      for (int i = 0; i < threads_; ++i) {
        threads.emplace_back([chunk_size = chunk_size_, &allocator, &mutex, &result] {
          MultiLineReaderV2 reader;
          while (!allocator.IsEmpty()) {
            reader.Parse(allocator.Allocate(chunk_size),   //
                         allocator.Allocate(chunk_size),   //
                         allocator.Allocate(chunk_size));  //
          }

          std::lock_guard _(mutex);
          // auto all_statistics = reader.GetStatistics().bucket |
          //                       std::views::filter([](const auto& item) { return item.used; }) |
          //                       std::views::transform([](const auto& item) {
          //                         return std::tuple<StationName, Statistics>{item.key, item.value};
          //                       });
          for (const auto& [name, statistics] : reader.GetStatistics()) {
            auto& [min, max, count, sum] = result[name];
            min = std::min(min, statistics.min);
            max = std::max(max, statistics.max);
            count += statistics.count;
            sum += statistics.sum;
          }
        });
      }
    }

    return result;
  }

 private:
  int threads_{1};
  size_t chunk_size_{1 << 21};  // 10MB by default
  std::span<const char> data_;
};
