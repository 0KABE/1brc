//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once

#include <concepts>
#include <span>

#include "data.h"
#include "find_byte.h"
#include "number_reader.h"

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
      // auto index = FindByte<FindFirstZeroByte, ';'>{}(span);
      //TODO: use template for memchr
      const auto pos = static_cast<const char *>(memchr(span.data(), ';', span.size_bytes()));
      const auto index = pos - span.data();
      name = std::string_view{span.first(index)};
      span = span.subspan(index + 1);
    }

    auto [size, temp] = NumberReader{}(span);
    span = span.subspan(size + 1);

    return {.name = name, .temperature = temp};
  }
};

template <FindLowestZeroByteFunctor FindFirstZeroByte, NumberReaderFunctor NumberReader>
struct SingleLineReaderParallelism {
  static std::tuple<Entity, Entity, Entity> operator()(ReaderBuffer &span1, ReaderBuffer &span2, ReaderBuffer &span3) {
    StationName name1;
    StationName name2;
    StationName name3;

    // find ';'
    {
      auto ind1 = FindByte<FindFirstZeroByte, ';'>{}(span1);
      auto ind2 = FindByte<FindFirstZeroByte, ';'>{}(span2);
      auto ind3 = FindByte<FindFirstZeroByte, ';'>{}(span3);

      name1 = std::string_view{span1.first(ind1)};
      name2 = std::string_view{span2.first(ind2)};
      name3 = std::string_view{span3.first(ind3)};

      span1 = span1.subspan(ind1 + 1);
      span2 = span2.subspan(ind2 + 1);
      span3 = span3.subspan(ind3 + 1);
    }

    auto [size1, temp1] = NumberReader{}(span1);
    auto [size2, temp2] = NumberReader{}(span2);
    auto [size3, temp3] = NumberReader{}(span3);

    span1 = span1.subspan(size1 + 1);
    span2 = span2.subspan(size2 + 1);
    span3 = span3.subspan(size3 + 1);

    return {
        {.name = name1, .temperature = temp1},
        {.name = name2, .temperature = temp2},
        {.name = name3, .temperature = temp3},
    };
  }
};

using SingleLineReader_V1 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_Base>;
using SingleLineReader_V2 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_SWAR_V1>;
using SingleLineReader_V3 = SingleLineReader<FindLowestZeroByte_Base, NumberReader_SWAR_V2>;
using SingleLineReader_V4 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_Base>;
using SingleLineReader_V5 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_SWAR_V1>;
using SingleLineReader_V6 = SingleLineReader<FindLowestZeroByte_SWAR, NumberReader_SWAR_V2>;
