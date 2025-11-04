//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

Entity ParseOnce_Base(std::span<const char>& span) {
  Entity entity{};
  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] == ';') {
      entity.name = std::string_view(span.first(i));
      span = span.subspan(i + 1);
      break;
    }
  }

  const bool negative = span.front() == '-';
  if (negative) {
    span = span.subspan(1);
  }

  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] == '.') {
      assert(i + 1 < span.size());
      entity.temperature = entity.temperature * 10 + span[i + 1] - '0';
      span = span.subspan(i + 3);
      break;
    }
    entity.temperature = entity.temperature * 10 + span[i] - '0';
  }
  if (negative) {
    entity.temperature = -entity.temperature;
  }
  return entity;
}

Entity ParseOnceV1(std::span<const char>& span) {

}
