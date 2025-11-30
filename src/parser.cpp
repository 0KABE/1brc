//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <fmt/format.h>

#include <bit>
#include <cassert>
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

Entity ParseOnce_V1(std::span<const char>& span) {
  std::string_view name;
  // find ';'
  {
    constexpr auto mask_semicolon = Mask(';');
    int size = 0;
    while (size < span.size()) {
      const auto n = *reinterpret_cast<const uint64_t*>(&span[size]);
      if (const auto index = FindFirstZeroByte_SWAR(n ^ mask_semicolon); index < sizeof(uint64_t)) {
        size += index;
        name = std::string_view{span.first(size)};
        span = span.subspan(size + 1);
        break;
      }
      size += sizeof(uint64_t);
    }
  }

  auto [size, temp] = ParseNumber_SWAR_V2(span);
  span = span.subspan(size + 1);

  return {.name = name, .temperature = temp};
}
