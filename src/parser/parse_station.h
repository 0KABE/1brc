//
// Created by Chen, WenTao on 2025/9/7.
//

#pragma once

#include <immintrin.h>

#include <cassert>
#include <cstring>
#include <span>
#include <string_view>

#include "parse_number.h"
#include "structs.h"
#include "utils/file_memory_map.h"

inline std::tuple<std::string_view, Temperature> ParseStation_V1(const char *&ptr) {
  std::string_view name;
  {
    const char *end = ptr;
    while (*++end != ';') {
    }
    name = std::string_view(ptr, end);
    ptr = end + 1;
  }

  int temperature = 0;
  bool negative = false;

  if (*ptr == '-') {
    negative = true;
    ++ptr;
  }

  for (char c = *ptr++; c != '.'; c = *ptr++) {
    temperature = temperature * 10 + (c - '0');
  }

  for (char c = *ptr++; c != '\n'; c = *ptr++) {
    temperature = temperature * 10 + (c - '0');
  }

  if (negative) {
    temperature = -temperature;
  }

  return {name, temperature};
}

inline std::tuple<std::string_view, Temperature> ParseStation_V2(const char *&ptr) {
  std::string_view name;
  int temperature;

  // --- 1. 使用AVX2向量化查找分号';' ---
  {
    // 创建一个所有字节都是';'的32字节向量
    const __m256i semicolon_vec = _mm256_set1_epi8(';');

    // 从当前指针位置加载32字节数据（非对齐加载）
    const __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr));

    // 并行比较32个字节，看哪个字节等于';'
    const __m256i comparison_mask = _mm256_cmpeq_epi8(chunk, semicolon_vec);

    // 将比较结果（一个32字节的掩码）转换为一个32位的整数位图(bitmap)
    uint32_t mask = _mm256_movemask_epi8(comparison_mask);

    // 查找位图中第一个为1的比特位，其索引就是';'的位置
    // _tzcnt_u32是快速找到末尾零个数的指令，效率很高
    const int semicolon_pos = _tzcnt_u32(mask);

    name = std::string_view(ptr, semicolon_pos);
    ptr += semicolon_pos + 1;
  }

  // --- 2. 使用位运算技巧（SWAR）无分支解析温度 ---
  {
    // 复制指针，避免修改原始ptr
    const char *p = ptr;

    // 处理负号
    const int sign_multiplier = (*p == '-') ? -1 : 1;
    p += (*p == '-');  // 如果是负号，指针前进一位

    // 一次性读取最多4个字节（例如 "12.3" 或 "9.8"）
    // memcpy可以避免非对齐访问问题，且通常会被编译器优化为一次读取
    uint32_t four_bytes;
    std::memcpy(&four_bytes, p, sizeof(four_bytes));

    // 小数点的位置决定了数字的格式
    // 检查第二个字节是否是'.' (e.g., "9.8")
    const int dot_pos = (four_bytes >> 8) & 0xFF;

    if (dot_pos == '.') {  // 格式: d.d (例如 9.8)
      // 提取个位数和十分位数
      const int d1 = (four_bytes & 0xFF) - '0';
      const int d2 = (four_bytes >> 16) & 0xFF - '0';
      temperature = (d1 * 10 + d2) * sign_multiplier;
      ptr = p + 3;  // 跳过 "d.d"
    } else {        // 格式: dd.d (例如 12.3)
      // 提取十位数、个位数和十分位数
      const int d1 = (four_bytes & 0xFF) - '0';
      const int d2 = (four_bytes >> 8) & 0xFF - '0';
      const int d3 = (four_bytes >> 24) & 0xFF - '0';
      temperature = (d1 * 100 + d2 * 10 + d3) * sign_multiplier;
      ptr = p + 4;  // 跳过 "dd.d"
    }
    ptr++;  // 跳过最后的换行符 '\n'
  }

  return {name, temperature};
}

inline int AVX256Find(const std::span<const char> span, const char target) {
  const __m256i semicolon_vec = _mm256_set1_epi8(target);
  const __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(span.data()));
  const __m256i comparison_mask = _mm256_cmpeq_epi8(chunk, semicolon_vec);
  const uint32_t mask = _mm256_movemask_epi8(comparison_mask);
  const int semicolon_pos = _tzcnt_u32(mask);
  return semicolon_pos;
}

inline std::tuple<std::string_view, Temperature> ParseStation_V3(std::span<const char> &span) {
  // assume the station name is less than 32 bytes
  const int semicolon_pos = AVX256Find(span, ';');
  auto name = std::string_view(span.data(), semicolon_pos);
  span = span.subspan(semicolon_pos + 1);

  // const int new_line_pos = AVX256Find(span, '\n');
  auto [temperature, n] = parse_number_v2(span);
  span = span.subspan(n + 1);

  return {name, temperature};
}
