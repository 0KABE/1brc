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

template <FindLowestZeroByteFunctor find_first_zero_byte, NumberReaderFunctor number_reader>
struct SingleLineReader {
  static Entity operator()(std::span<const char> &span) {
    std::string_view name;
    // find ';'
    {
      constexpr auto mask_semicolon = Mask(';');
      int size = 0;
      while (size < span.size()) {
        const auto n = *reinterpret_cast<const uint64_t *>(&span[size]);
        if (const auto index = find_first_zero_byte{}(n ^ mask_semicolon); index < sizeof(uint64_t)) {
          size += index;
          name = std::string_view{span.first(size)};
          span = span.subspan(size + 1);
          break;
        }
        size += sizeof(uint64_t);
      }
    }

    auto [size, temp] = number_reader{}(span);
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
