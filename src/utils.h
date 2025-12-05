//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <bit>
#include <climits>
#include <cstdint>
#include <cstring>
#include <span>

constexpr uint64_t Mask(const char c) {
  constexpr auto bytes = sizeof(uint64_t);
  uint64_t mask = 0;
  for (auto i = 0; i < bytes; ++i) {
    mask = (mask << CHAR_BIT) | c;
  }
  return mask;
}

constexpr uint64_t ReadAsUint64(const std::span<const char> span) {
  uint64_t result = 0;
  std::memcpy(&result, span.data(), sizeof(uint64_t));
  return result;
}

template <typename F>
concept FindLowestZeroByteFunctor = requires(F f, uint64_t x) {
  { f(x) } -> std::convertible_to<int>;
};

struct FindLowestZeroByte_Base {
  static constexpr int operator()(uint64_t x) {
    for (auto i = 0; i < sizeof(uint64_t); ++i) {
      if (!(x & 0xFF)) {
        return i;
      }
      x >>= CHAR_BIT;
    }
    return sizeof(uint64_t);
  }
};

struct FindLowestZeroByte_SWAR {
  static constexpr int operator()(const uint64_t x) {
    const uint64_t zero_mask = (x - 0x0101010101010101) & ~x & 0x8080808080808080;
    return std::countr_zero(zero_mask) / 8;
  }
};

template <FindLowestZeroByteFunctor FindFirstZeroByte, char Byte>
struct FindByte {
  static constexpr int operator()(const std::span<const char> span) {
    constexpr auto mask_semicolon = Mask(Byte);
    int size = 0;
    while (size < span.size()) {
      const auto n = *reinterpret_cast<const uint64_t *>(&span[size]);
      if (const auto index = FindFirstZeroByte{}(n ^ mask_semicolon); index < sizeof(uint64_t)) {
        size += index;
        return size;
      }
      size += sizeof(uint64_t);
    }
    return size;
  }
};