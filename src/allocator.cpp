//
// Created by Chen, WenTao on 2025/12/6.
//

#include "allocator.h"

#include "find_byte.h"

std::span<const char> Allocator::Allocate(size_t size) {
  std::lock_guard _(mutex_);

  if (size >= buff_.size()) {
    const auto span = buff_;
    buff_ = std::span<const char>{};
    return span;  // NOLINT
  }

  size += FindByte<FindLowestZeroByte_SWAR, '\n'>{}(buff_.subspan(size));
  const auto span = buff_.first(size + 1);
  buff_ = buff_.subspan(size + 1);
  return span;  // NOLINT
}

bool Allocator::IsEmpty() const {
  std::lock_guard _(mutex_);
  return buff_.empty();
}