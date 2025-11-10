//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <fmt/format.h>

#include <bit>
#include <cstdint>

Entity ParseOnce_Base(std::span<const char>& span) {
  Entity entity{};
  size_t index = 0;

  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] == ';') {
      entity.name = std::string_view(span.first(i));
      index = i + 1;
      break;
    }
  }

  const bool negative = span[index] == '-';
  index += negative;

  for (size_t i = index; i < span.size(); ++i) {
    if (span[i] == '.') {
      assert(i + 1 < span.size());
      entity.temperature = static_cast<Temperature>(entity.temperature * 10 + (span[i + 1] - '0'));
      index = i + 3;
      break;
    }
    entity.temperature = static_cast<Temperature>(entity.temperature * 10 + (span[i] - '0'));
  }

  if (negative) {
    entity.temperature = static_cast<Temperature>(-entity.temperature);
  }

  span = span.subspan(index);
  return entity;
}

constexpr uint64_t Mask(const char c) {
  constexpr auto bytes = sizeof(uint64_t);
  uint64_t mask = 0;
  for (auto i = 0; i < bytes; ++i) {
    mask = (mask << CHAR_BIT) | c;
  }
  return mask;
}

int FindFirstZeroByte(const uint64_t x) {
  const uint64_t zero_mask = (x - 0x0101010101010101) & ~x & 0x8080808080808080;
  return std::countr_zero(zero_mask) / 8;
}

int FindFirstZeroByte_Base(uint64_t x) {
  for (auto i = 0; i < sizeof(uint64_t); ++i) {
    if (!(x & 0xFF)) {
      return i;
    }
    x >>= CHAR_BIT;
  }
  return sizeof(uint64_t);
}

Entity ParseOnceV1(std::span<const char>& span) {
  constexpr auto m = Mask('-');
  fmt::println("mask: {}", m);
  return {};
}
