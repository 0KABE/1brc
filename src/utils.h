//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <climits>
#include <cstdint>

constexpr uint64_t Mask(const char c) {
  constexpr auto bytes = sizeof(uint64_t);
  uint64_t mask = 0;
  for (auto i = 0; i < bytes; ++i) {
    mask = (mask << CHAR_BIT) | c;
  }
  return mask;
}
