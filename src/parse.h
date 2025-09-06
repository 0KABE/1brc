//
// Created by Chen, WenTao on 2025/8/19.
//

#pragma once

#include <cstdint>

constexpr uint64_t filter_zero_bytes(uint64_t v) {
  v = v - 0x0101010101010101 & ~v & 0x8080808080808080;
  return v;
}
