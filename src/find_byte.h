//
// Created by Chen, WenTao on 2025/12/25.
//

#pragma once

#include <bit>
#include <cassert>
#include <climits>
#include <concepts>
#include <cstdint>
#include <cstring>

#include "data.h"
#include "utils.h"

template <typename F>
concept FindLowestZeroByteFunctor = requires(F f, uint64_t x) {
  { f(x) } -> std::convertible_to<int>;
};

struct FindLowestZeroByte_Base {
  static constexpr int operator()(uint64_t x) {
    for (auto i = 0; i < sizeof(uint64_t); ++i) {
      if (!(x & 0xFF)) {
        return i;
      }
      x >>= CHAR_BIT;
    }
    return sizeof(uint64_t);
  }
};

struct FindLowestZeroByte_SWAR {
  static constexpr int operator()(const uint64_t x) {
    const uint64_t zero_mask = (x - 0x0101010101010101) & ~x & 0x8080808080808080;
    return std::countr_zero(zero_mask) / 8;
  }
};

struct UseStd {};

template <typename Strategy, char Byte>
struct FindByte;

template <template <char> class F, char Byte>
concept FindByteFunctor = requires(ReaderBuffer buffer) {
  { F<Byte>{}(buffer) } -> std::convertible_to<int>;
};

template <FindLowestZeroByteFunctor FindFirstZeroByte, char Byte>
struct FindByte<FindFirstZeroByte, Byte> {
  static constexpr int operator()(const ReaderBuffer buffer) {
    constexpr auto mask_semicolon = Mask(Byte);
    int size = 0;
    while (size < buffer.size()) {
      const auto n = std::bit_cast<uint64_t>(buffer.subspan(size).first<sizeof(uint64_t)>());
      if (const auto index = FindFirstZeroByte{}(n ^ mask_semicolon); index < sizeof(uint64_t)) {
        size += index;
        return size;
      }
      size += sizeof(uint64_t);
    }
    return size;
  }
};

template <char Byte>
struct FindByte<UseStd, Byte> {
  static int operator()(const ReaderBuffer buffer) {
    const void* ptr = std::memchr(buffer.data(), Byte, buffer.size());
    assert(ptr != nullptr);
    return static_cast<int>(static_cast<const char*>(ptr) - buffer.data());
  }
};

template <char Byte>
using FindByte_Base = FindByte<FindLowestZeroByte_Base, Byte>;
template <char Byte>
using FindByte_SWAR = FindByte<FindLowestZeroByte_SWAR, Byte>;
template <char Byte>
using FindByte_STD = FindByte<UseStd, Byte>;
