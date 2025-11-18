//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once

#include <climits>
#include <cstring>
#include <span>
#include <tuple>

#include "data.h"

Entity ParseOnce_Base(std::span<const char> &span);

Entity ParseOnceV1(std::span<const char> &span);

constexpr uint64_t Mask(const char c) {
  constexpr auto bytes = sizeof(uint64_t);
  uint64_t mask = 0;
  for (auto i = 0; i < bytes; ++i) {
    mask = (mask << CHAR_BIT) | c;
  }
  return mask;
}

constexpr int FindFirstZeroByte_Base(uint64_t x) {
  for (auto i = 0; i < sizeof(uint64_t); ++i) {
    if (!(x & 0xFF)) {
      return i;
    }
    x >>= CHAR_BIT;
  }
  return sizeof(uint64_t);
}

constexpr int FindFirstZeroByte_SWAR(const uint64_t x) {
  const uint64_t zero_mask = (x - 0x0101010101010101) & ~x & 0x8080808080808080;
  return std::countr_zero(zero_mask) / 8;
}

constexpr uint64_t ReadAsUint64(const std::span<const char> span) {
  uint64_t result = 0;
  std::memcpy(&result, span.data(), sizeof(uint64_t));
  return result;
}

constexpr std::tuple<int, int> ParseNumber_Base(const std::span<const char> span) {
  int number = 0;
  int index = 0;

  const bool negative = span[index] == '-';
  index += negative;

  for (auto c = span[index]; span[index++] != '.'; c = span[index]) {
    number = number * 10 + c - '0';
  }
  number = number * 10 + span[index++] - '0';
  number = negative ? -number : number;
  return {index, number};
}

constexpr std::tuple<int, int> ParseNumber_SWAR(std::span<const char> span) {
  const bool negative = span[0] == '-';
  span = span.subspan(negative);

  auto n = ReadAsUint64(span);
  const auto number_len = FindFirstZeroByte_SWAR(n ^ Mask('.')) + 2;
  n <<= (sizeof(uint64_t) - number_len) * CHAR_BIT;
  n >>= 4 * CHAR_BIT;
  n &= 0x0F000F0F;
  const auto number = (n * 0x640A000100) >> 32 & 0x03FF;
  return {number_len + negative, negative ? -number : number};
}
