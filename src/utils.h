//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <climits>
#include <cstdint>
#include <span>

constexpr uint64_t Mask(const char c) {
  constexpr auto bytes = sizeof(uint64_t);
  uint64_t mask = 0;
  for (auto i = 0; i < bytes; ++i) {
    mask = (mask << CHAR_BIT) | c;
  }
  return mask;
}

template <typename T>
constexpr T BitCast(const std::span<const char> span) {
  T v;
  memcpy(&v, span.data(), sizeof(T));
  return v;
}
