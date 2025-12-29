//
// Created by Chen, WenTao on 2025/11/2.
//

#pragma once

#include <limits>
#include <string_view>

#include "hash.h"

using Temperature = int16_t;
using ReaderBuffer = std::span<const char>;

struct StationName {
  size_t hash{};
  std::string_view name;

  StationName() = default;
  // StationName(const std::string_view name) : hash(std::hash<std::string_view>{}(name)), name(name) {}
  StationName(const std::string_view name) : hash(FastHash(name)), name(name) {}
  // StationName(const std::string_view name) : hash(SimpleHash(name)), name(name) {}

  bool operator==(const StationName& other) const { return hash == other.hash; }
  auto operator<=>(const StationName& other) const { return name <=> other.name; }
};

struct Entity {
  StationName name;           // 1 - 100 Bytes
  Temperature temperature{};  // -99.9 - 99.9

  bool operator==(const Entity& other) const { return name == other.name && temperature == other.temperature; }
};

struct Statistics {
  Temperature min{std::numeric_limits<Temperature>::max()};
  Temperature max{std::numeric_limits<Temperature>::min()};
  int count{0};
  size_t sum{0};

  void Record(const Temperature temp) {
    min = std::min(min, temp);
    max = std::max(max, temp);
    sum += temp;
    ++count;
  }
};
