//
// Created by Chen, WenTao on 2025/12/6.
//

#pragma once

#include <algorithm>
#include <span>

#include "utils.h"

class Allocator {
 public:
  explicit Allocator(const std::span<const char> span) : buff_(span) {}

  std::span<const char> Allocate(size_t size) {
    if (size >= buff_.size()) {
      const auto span = buff_;
      buff_ = std::span<const char>{};
      return span;
    }

    size += FindByte<FindLowestZeroByte_SWAR, '\n'>{}(buff_.subspan(size));
    const auto span = buff_.first(size + 1);
    buff_ = buff_.subspan(size + 1);
    return span;
  }

 private:
  std::span<const char> buff_;
};
