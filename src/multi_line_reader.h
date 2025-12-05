//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <unordered_map>

#include "single_line_reader.h"

template <SingleLineReaderFunctor Reader>
class MultiLineReader {
 public:
  using StatisticsMap = std::unordered_map<StationName, Statistics>;

  void Parse(std::span<const char> span) {
    while (!span.empty()) {
      const auto [name, temperature] = Reader{}(span);

      auto& [min, max, count, sum] = statistics_[name];
      min = std::min(min, temperature);
      max = std::max(max, temperature);
      sum += temperature;
      ++count;
    }
  }

  [[nodiscard]] const StatisticsMap& GetStatistics() const { return statistics_; }

 private:
  StatisticsMap statistics_;
};
