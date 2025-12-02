//
// Created by Chen, WenTao on 2025/12/3.
//

#pragma once

#include <span>
#include <tuple>

#include "data.h"

using ByteCount = int;
using NumberReaderResult = std::tuple<ByteCount, Temperature>;

template <typename F>
concept NumberReadable = requires(F f, std::span<const char> span) {
  { f(span) } -> std::convertible_to<NumberReaderResult>;
};

struct NumberReader_Base {
  static NumberReaderResult operator()(std::span<const char> span);
};

struct NumberReader_SWAR_V1 {
  static NumberReaderResult operator()(std::span<const char> span);
};

struct NumberReader_SWAR_V2 {
  static NumberReaderResult operator()(std::span<const char> span);
};
