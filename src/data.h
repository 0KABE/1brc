//
// Created by Chen, WenTao on 2025/11/2.
//

#pragma once

#include <cstdint>
#include <limits>
#include <string_view>
#include <unordered_map>

using StationName = std::string_view;
using Temperature = int16_t;
using ReaderBuffer = std::span<const char>;

struct Entity {
  StationName name;         // 1 - 100 Bytes
  Temperature temperature;  // -99.9 - 99.9

  bool operator==(const Entity& other) const { return name == other.name && temperature == other.temperature; }
};

struct Statistics {
  Temperature min{std::numeric_limits<Temperature>::max()};
  Temperature max{std::numeric_limits<Temperature>::min()};
  int count{0};
  size_t sum{0};
};

using MultiLineReaderStatistics = std::unordered_map<StationName, Statistics>;
