//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once

#include <climits>
#include <span>
#include <tuple>

#include "data.h"
#include "utils.h"

constexpr std::tuple<int, Temperature> ParseNumber_Base(const std::span<const char> span) {
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

constexpr std::tuple<int, Temperature> ParseNumber_SWAR_V1(std::span<const char> span) {
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

// Parses a temperature value using SWAR (SIMD Within A Register) techniques.
// The format is assumed to be one of: d.d, dd.d, -d.d, -dd.d.
// Returns a tuple containing the number of characters parsed and the temperature value as an integer (e.g., -12.3 is
// returned as -123).
inline std::tuple<int, Temperature> ParseNumber_SWAR_V2(const std::span<const char> span) {
  // Load 8 bytes into a 64-bit integer for parallel processing.
  const uint64_t v = *reinterpret_cast<const uint64_t *>(span.data());

  // Branchless check for a negative sign. `negative` becomes -1 if the first char is '-' (0x2D), otherwise 0.
  // Only the 5th least significant bit of '-' is 1; for other characters, it is 0.
  // ASCII Table
  // '-' : 0x2D 0b0010 1101
  // '0' : 0x30 0b0011 0000
  // ...
  // '9' : 0x39 0b0011 1001
  const int64_t negative = static_cast<int64_t>(~v) << 59 >> 63;

  // Branchlessly discard the sign character by shifting it out if present.
  uint64_t pure_number = v >> (CHAR_BIT * -negative);

  // Check if the number has one or two digits before the decimal point.
  // `ten` is 1 if the format is `d.d` (dot at the second position), 0 if `dd.d`.
  const uint64_t ten = ((~pure_number ^ 0x2e00) & 0xFF00) == 0xFF00;

  // Align the digits to a canonical layout. If the format is `d.d`,
  // this shift attempts to place the digits in the same byte positions as the `dd.d` format.
  pure_number <<= ten * 8;

  // Mask to extract the integer value from ASCII digit characters.
  // '0' (0x30) & 0x0F = 0, '9' (0x39) & 0x0F = 9.
  // This assumes digits are in specific byte locations after alignment.
  pure_number &= 0x0F000F0F;

  // Calculate the total number of characters consumed by the number string (e.g., "-12.3" is 5 chars).
  const int n = 4 + static_cast<int>(-negative - ten);

  // This is a SWAR dot-product calculation. The magic number 0x640A000100 is equivalent to
  // (100 * 2^32 + 10 * 2^24 + 1 * 2^8). Multiplying the packed digits by this magic number
  // and right-shifting computes `d1*100 + d2*10 + d3` in a single operation.
  const uint64_t number = (pure_number * 0x640A000100) >> 32 & 0x03FF;

  // Apply the sign branchlessly. (number ^ negative) - negative is equivalent to
  // -number if negative is -1, and number if negative is 0.
  const int result = static_cast<int>((number ^ negative) - negative);
  return {n, result};
}

Entity ParseOnce_Base(std::span<const char> &span);

template <int (*FindFirstZeroByte)(uint64_t), std::tuple<int, Temperature> (*ParseNumber)(std::span<const char>)>
Entity ParseOnce(std::span<const char> &span) {
  std::string_view name;
  // find ';'
  {
    constexpr auto mask_semicolon = Mask(';');
    int size = 0;
    while (size < span.size()) {
      const auto n = *reinterpret_cast<const uint64_t *>(&span[size]);
      if (const auto index = FindFirstZeroByte(n ^ mask_semicolon); index < sizeof(uint64_t)) {
        size += index;
        name = std::string_view{span.first(size)};
        span = span.subspan(size + 1);
        break;
      }
      size += sizeof(uint64_t);
    }
  }

  auto [size, temp] = ParseNumber(span);
  span = span.subspan(size + 1);

  return {.name = name, .temperature = temp};
}
