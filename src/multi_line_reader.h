//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <algorithm>
#include <ranges>

#include "single_line_reader.h"

template <typename F>
concept MultiLineReaderFunctor = requires(F f, ReaderBuffer buff) {
  { f.Parse(buff) } -> std::convertible_to<void>;
  { f.GetStatistics() } -> std::convertible_to<const MultiLineReaderStatistics&>;
};

template <SingleLineReaderFunctor Reader>
class MultiLineReader {
 public:
  void Parse(ReaderBuffer span) {
    while (!span.empty()) {
      const auto [name, temperature] = Reader{}(span);
      statistics_[name].Record(temperature);
    }
  }

  [[nodiscard]] const MultiLineReaderStatistics& GetStatistics() const { return statistics_; }

 private:
  MultiLineReaderStatistics statistics_;
};

class MultiLineReaderV2 {
 public:
  using StatisticsMap = std::unordered_map<StationName, Statistics>;

  void Parse(ReaderBuffer buff1, ReaderBuffer buff2, ReaderBuffer buff3) {
    while (!buff1.empty() && !buff2.empty() && !buff3.empty()) {
      const auto [e1, e2, e3] =
          SingleLineReaderParallelism<FindLowestZeroByte_SWAR, NumberReader_SWAR_V2>{}(buff1, buff2, buff3);

      statistics_[e1.name].Record(e1.temperature);
      statistics_[e2.name].Record(e2.temperature);
      statistics_[e3.name].Record(e3.temperature);
    }

    while (!buff1.empty()) {
      const auto [name, temp] = SingleLineReader_V6{}(buff1);
      statistics_[name].Record(temp);
    }

    while (!buff2.empty()) {
      const auto [name, temp] = SingleLineReader_V6{}(buff2);
      statistics_[name].Record(temp);
    }

    while (!buff3.empty()) {
      const auto [name, temp] = SingleLineReader_V6{}(buff3);
      statistics_[name].Record(temp);
    }
  }

  [[nodiscard]] const MultiLineReaderStatistics& GetStatistics() const { return statistics_; }

 private:
  MultiLineReaderStatistics statistics_;
};
