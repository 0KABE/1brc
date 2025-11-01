//
// Created by Chen, WenTao on 2025/9/9.
//

#pragma once

#include <mutex>
#include <print>
#include <span>

template <bool EnableInfiniteLoop = false>
class ContiguousAllocator {
 public:
  using Buffer = std::span<const char>;

  ContiguousAllocator() = default;

  explicit ContiguousAllocator(const Buffer buffer) : full_buffer_(buffer), remaining_(buffer) {}

  ContiguousAllocator(ContiguousAllocator&& other) noexcept {
    if (this != &other) {
      *this = std::move(other);
    }
  }

  ContiguousAllocator& operator=(ContiguousAllocator&& other) noexcept {
    ContiguousAllocator empty;
    Swap(empty);
    Swap(other);
    return *this;
  }

  std::span<const char> Allocate(size_t size) {
    std::lock_guard _(mutex_);

    if constexpr (EnableInfiniteLoop) {
      if (remaining_.empty() && !full_buffer_.empty()) {
        remaining_ = full_buffer_;
      }
    }

    size = std::min(size, remaining_.size());

    while (size < remaining_.size() && remaining_[size++] != '\n') {
    }
    const auto allocated = remaining_.first(size);
    remaining_ = remaining_.subspan(size);
    // std::println("reaming size: {}", remaining_.size());
    return allocated;
  }

  bool Empty() const {
    std::lock_guard _(mutex_);
    return remaining_.empty();
  }

  void Swap(ContiguousAllocator& other) {
    std::scoped_lock _(mutex_, other.mutex_);
    std::swap(full_buffer_, other.full_buffer_);
    std::swap(remaining_, other.remaining_);
  }

 private:
  mutable std::mutex mutex_;
  Buffer full_buffer_;
  Buffer remaining_;
};