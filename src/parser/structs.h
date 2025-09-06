//
// Created by Chen, WenTao on 2025/8/21.
//

#pragma once

#include <limits>
#include <string>
#include <unordered_map>

using Temperature = int;

struct Station {
  std::string name;
  Temperature temperature;
};

struct StationStatistics {
  int count{0};
  int min{std::numeric_limits<int>::max()};
  int max{std::numeric_limits<int>::min()};
  int64_t sum{0};
};

using Statistics = std::unordered_map<std::string_view, StationStatistics>;
