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
