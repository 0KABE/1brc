//
// Created by Chen, WenTao on 2025/11/2.
//

#pragma once

#include <cstdint>
#include <string_view>

using StationName = std::string_view;
using Temperature = int16_t;

struct Entity {
  StationName name;         // 1 - 100 Bytes
  Temperature temperature;  // -99.9 - 99.9

  bool operator==(const Entity& other) const { return name == other.name && temperature == other.temperature; }
};
