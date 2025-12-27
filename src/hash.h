//
// Created by Chen, WenTao on 2025/12/25.
//

#pragma once

#include <climits>
#include <cstdint>
#include <cstring>
#include <span>

#include "utils.h"

inline size_t SimpleHash(std::span<const char> buff) {
  size_t result = 0;

  while (buff.size_bytes() >= CHAR_BIT) {
    const auto v = BitCast<size_t>(buff);
    buff = buff.subspan<CHAR_BIT>();
    result ^= v;
  }

  size_t v = 0;
  memcpy(&v, buff.data(), buff.size_bytes());
  result ^= v;

  return result;
}

// 核心：MUM 混合函数 (Multiply and Mix)
// 将两个 64 位数相乘，并将其高低位进行异或
inline uint64_t WyMix(const uint64_t a, const uint64_t b) {
  __uint128_t r = a;
  r *= b;
  return static_cast<uint64_t>(r) ^ static_cast<uint64_t>(r >> 64);
}

inline uint64_t FastHash(const std::span<const char> buff) {
  const auto* p = reinterpret_cast<const uint8_t*>(buff.data());
  size_t len = buff.size();
  const uint64_t seed = len;  // 长度作为初始种子，防止 "abc" 和 "abc\0" 冲突

  // 常用随机质数常量
  constexpr uint64_t _p0 = 0xa0761d6478bd642fULL;
  constexpr uint64_t _p1 = 0xe7037ed1a0b428dbULL;

  // 分情况处理长度，利用 SWAR (SIMD within a register)
  if (len <= 8) {
    if (len >= 4) {
      // 读取前4字节和后4字节（可能重叠）
      uint32_t first4;
      uint32_t last4;
      std::memcpy(&first4, p, 4);
      std::memcpy(&last4, p + len - 4, 4);
      return WyMix(_p0 ^ len, WyMix(first4 ^ _p1, last4 ^ seed));
    } else if (len > 0) {
      // 处理 1-3 字节
      const uint8_t a = p[0];
      const uint8_t b = p[len >> 1];
      const uint8_t c = p[len - 1];
      return WyMix(_p0 ^ len, WyMix(a ^ _p1, (static_cast<uint32_t>(b) << 8) | c));
    }
    return 0;
  }

  // 处理 8 字节以上
  uint64_t h = seed ^ _p0;
  while (len > 8) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    h = WyMix(h ^ _p1, v);
    p += 8;
    len -= 8;
  }

  // 处理最后的 1-8 字节（尾部对齐读取）
  uint64_t tail;
  std::memcpy(&tail, p + len - 8, 8);
  return WyMix(h, tail ^ _p1);
}
