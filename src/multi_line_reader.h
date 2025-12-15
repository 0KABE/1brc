//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <algorithm>
#include <ranges>
#include <unordered_map>

#include "single_line_reader.h"

using ReaderBuffer = std::span<const char>;
using MultiLineReaderStatistics = std::unordered_map<StationName, Statistics>;

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

      auto& [min, max, count, sum] = statistics_[name];
      min = std::min(min, temperature);
      max = std::max(max, temperature);
      sum += temperature;
      ++count;
    }
  }

  [[nodiscard]] const MultiLineReaderStatistics& GetStatistics() const { return statistics_; }

 private:
  MultiLineReaderStatistics statistics_;
};

template <SingleLineReaderFunctor Reader>
class MultiLineReaderV2 {
 public:
  using StatisticsMap = std::unordered_map<StationName, Statistics>;

  void Parse(ReaderBuffer buff1, ReaderBuffer buff2, ReaderBuffer buff3) {
    while (!buff1.empty() && !buff2.empty() && !buff3.empty()) {
      const auto [name1, temp1] = Reader{}(buff1);
      const auto [name2, temp2] = Reader{}(buff2);
      const auto [name3, temp3] = Reader{}(buff3);

      auto& s1 = statistics_[name1];
      auto& s2 = statistics_[name2];
      auto& s3 = statistics_[name3];

      s1.min = std::min(s1.min, temp1);
      s1.max = std::max(s1.max, temp1);
      s1.sum += temp1;
      ++s1.count;

      s2.min = std::min(s2.min, temp2);
      s2.max = std::max(s2.max, temp2);
      s2.sum += temp2;
      ++s2.count;

      s3.min = std::min(s3.min, temp3);
      s3.max = std::max(s3.max, temp3);
      s3.sum += temp3;
      ++s3.count;
    }

    while (!buff1.empty()) {
      const auto [name, temp] = Reader{}(buff1);
      auto& s = statistics_[name];
      s.min = std::min(s.min, temp);
      s.max = std::max(s.max, temp);
      s.sum += temp;
      ++s.count;
    }

    while (!buff2.empty()) {
      const auto [name, temp] = Reader{}(buff2);
      auto& s = statistics_[name];
      s.min = std::min(s.min, temp);
      s.max = std::max(s.max, temp);
      s.sum += temp;
      ++s.count;
    }

    while (!buff3.empty()) {
      const auto [name, temp] = Reader{}(buff3);
      auto& s = statistics_[name];
      s.min = std::min(s.min, temp);
      s.max = std::max(s.max, temp);
      s.sum += temp;
      ++s.count;
    }
  }

  [[nodiscard]] const MultiLineReaderStatistics& GetStatistics() const { return statistics_; }

 private:
  MultiLineReaderStatistics statistics_;
};
