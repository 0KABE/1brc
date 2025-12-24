//
// Created by Chen, WenTao on 2025/12/6.
//

#pragma once

#include <mutex>
#include <span>

#include "data.h"

class Allocator {
 public:
  explicit Allocator(const ReaderBuffer span) : buff_(span) {}

  ReaderBuffer Allocate(size_t size);

  bool IsEmpty() const;

 private:
  mutable std::mutex mutex_;
  std::span<const char> buff_;
};
