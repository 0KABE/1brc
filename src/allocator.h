//
// Created by Chen, WenTao on 2025/12/6.
//

#pragma once

#include <algorithm>
#include <mutex>
#include <span>

#include "find_byte.h"
#include "utils.h"

class Allocator {
 public:
  explicit Allocator(const std::span<const char> span) : buff_(span) {}

  std::span<const char> Allocate(size_t size) {
    std::lock_guard _(mutex_);

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

  bool IsEmpty() const {
    std::lock_guard _(mutex_);
    return buff_.empty();
  }

 private:
  mutable std::mutex mutex_;
  std::span<const char> buff_;
};
