//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once
#include <cassert>
#include <span>

#include "data.h"

inline Entity BasicParseOnce(std::span<const char> &span) {
  Entity entity{};
  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] == ';') {
      entity.name = std::string_view(span.first(i));
      span = span.subspan(i + 1);
      break;
    }
  }
  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] == '.') {
      assert(i + 1 < span.size());
      entity.temperature = entity.temperature * 10 + span[i + 1] - '0';
      span = span.subspan(i + 2);
      break;
    }
    entity.temperature = entity.temperature * 10 + span[i] - '0';
  }
  return entity;
}
