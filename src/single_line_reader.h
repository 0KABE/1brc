//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once

#include <concepts>
#include <span>

#include "data.h"
#include "number_reader.h"
#include "utils.h"

template <typename F>
concept SingleLineReaderFunctor = requires(F f, std::span<const char> span) {
  { f(span) } -> std::convertible_to<Entity>;
};

struct SingleLineReader_Base {
  static Entity operator()(std::span<const char> &span);
};

template <FindLowestZeroByteFunctor FindFirstZeroByte, NumberReaderFunctor NumberReader>
struct SingleLineReader {
  static Entity operator()(std::span<const char> &span) {
    StationName name;
    // find ';'
    {
      auto index = FindByte<FindFirstZeroByte, ';'>{}(span);
      name = std::string_view{span.first(index)};
      span = span.subspan(index + 1);
    }

    auto [size, temp] = NumberReader{}(span);
    span = span.subspan(size + 1);

    return {.name = name, .temperature = temp};
  }
};

using SingleLineReader_V1 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_Base>;
using SingleLineReader_V2 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_SWAR_V1>;
using SingleLineReader_V3 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_SWAR_V2>;
using SingleLineReader_V4 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_Base>;
using SingleLineReader_V5 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_SWAR_V1>;
using SingleLineReader_V6 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_SWAR_V2>;
