//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <fmt/format.h>

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

uint64_t FindFirstZeroByte(const uint64_t x) {
  constexpr uint64_t magicM = 0x7F7F7F7F7F7F7F7F;
  constexpr uint64_t magicH = 0x8080808080808080;

  // v & m1
  // 0x00 & 0x7F = 0x00
  // (0x01 ~ 0x7F) & 0x7F = (0x01 ~ 0x7F)
  // (0x80 ~ 0xFF) & 0x7F = (0x00 ~ 0x7F)
  //
  // v & m1 + m1 => (0x7F ~ 0xFE)
  const auto t = (x & magicM) + magicM;

  // 0x00
  // 0x80 ~ 0xFF
  const auto r = t | x;

  const auto result = ~(r & magicH) & magicH;

  return std::countr_zero(result) / 8 + 1;
}

Entity ParseOnceV1(std::span<const char>& span) {
  constexpr auto m = Mask('-');
  fmt::println("mask: {}", m);
  return {};
}
