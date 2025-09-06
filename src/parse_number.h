//
// Created by Chen, WenTao on 2025/9/7.
//

#pragma once

#include <climits>
#include <span>
#include <thread>

#include "structs.h"
#include "utils/file_memory_map.h"

constexpr uint64_t get_delimiter_mask(char c) {
  uint64_t v = 0;
  for (int i = 0; i < sizeof(uint64_t); ++i) {
    v = v << CHAR_BIT | c;
  }
  return v;
}

inline int parse_number_v1(const std::span<const char> span) {
  const uint64_t v = *reinterpret_cast<const uint64_t *>(span.data());
  constexpr auto sign_mask = get_delimiter_mask('-');
  const int negative = (static_cast<int>(v ^ sign_mask) & 0xFF) - 1;
  const uint64_t pure_number = (v >> 8 * negative) & 0x0F000F0F;
  const int number =
      (pure_number & 0x0F) * 100 + ((pure_number & 0x0F00) >> 8) * 10 + ((pure_number & 0x0F000000) >> 8 * 3);

  return negative ? -number : number;
}

inline std::tuple<Temperature, int> parse_number_v2(const std::span<const char> span) {
  const uint64_t v = *reinterpret_cast<const uint64_t *>(span.data());
  const int64_t negative = static_cast<int64_t>(~v) << 59 >> 63;
  uint64_t pure_number = v >> (CHAR_BIT * -negative);
  const uint64_t ten = ((~pure_number ^ 0x2e00) & 0xFF00) == 0xFF00;
  pure_number <<= ten * 8;
  pure_number &= 0x0F000F0F;

  const int n = 4 + static_cast<int>(-negative - ten);

  //   0x0C000B0A =   A * 2^0  +  B * 2^8  + C * 2^24
  // 0x640A000100 = 100 * 2^32 + 10 * 2^24 + 1 * 2^8
  const uint64_t number = (pure_number * 0x640A000100) >> 32 & 0x03FF;
  const int result = static_cast<int>((number ^ negative) - negative);
  return {result, n};
}

inline std::tuple<Temperature, int> parse_number_v3(const std::span<const char> span) {
  // 1. 一次性加载8个字节，这是所有SWAR技巧的基础
  const uint64_t v = *reinterpret_cast<const uint64_t *>(span.data());

  // 2. 无分支判断符号位。如果第一个字符是'-'，is_negative_mask为-1
  // (0xFF..FF)，否则为0。
  const int64_t is_negative_mask = static_cast<int64_t>(~v) << 59 >> 63;

  // 3. 根据符号，无分支地移除负号。
  // 如果存在负号, is_negative_mask 为 -1, (-is_negative_mask)为1, 右移8位。
  // 否则, is_negative_mask 为 0, 右移0位。
  uint64_t digits_word = v >> (CHAR_BIT * -is_negative_mask);

  // 4. 判断小数点位置，区分 "dd.d" 和 "d.d" 两种格式。
  // a. `~digits_word ^ 0x2e00`: 如果`digits_word`的第二个字节是'.' (0x2e)，
  //    则取反后与0x2e异或的结果在该字节会是0xFF。
  // b. `& 0xFF00`: 仅关注第二个字节的结果。
  // c. `== 0xFF00`: 判断是否匹配成功。
  // 这一步比原版的 `/ 0xFF00` 更直接且可能更快。
  const int is_short_form = (((~digits_word ^ 0x2e00) & 0xFF00) == 0xFF00);

  // 5. 根据格式对齐数字。如果是"d.d"格式，左移8位，变为" d.d"的内存布局，
  //    从而与"dd.d"格式的内存布局统一，方便后续统一处理。
  digits_word <<= (is_short_form * CHAR_BIT);

  // 6. 提取数字的数值。ASCII码的'0'-'9'与0x0F相与即可得到0-9。
  //    掩码 0x0F000F0F 精准地提取出 百/十位(byte 0)、个位(byte 1) 和
  //    小数位(byte 3) 的数值。
  const uint64_t digits = digits_word & 0x0F000F0F;

  // 7. SWAR核心：一次乘法并行计算出 `(百/十位*100) + (个位*10) + 小数位`
  //    魔法数字 0x640A000100ULL = (100ULL<<32) + (10ULL<<24) + (1ULL<<8)
  const uint64_t number = (digits * 0x640A000100ULL) >> 32;

  // 8. 无分支应用符号。 (val ^ 0) - 0 = val; (val ^ -1) - (-1) = ~val + 1 =
  // -val
  const int result = static_cast<int>((number ^ is_negative_mask) - is_negative_mask);

  // 9. 计算消耗的字符数。
  //    基础长度是 "dd.d" (4个字符)。
  //    负号多一个字符，is_negative_mask为-1, -(-1) = +1。
  //    "d.d"格式少一个字符, is_short_form为1, -1。
  const int n = 4 + static_cast<int>(-is_negative_mask - is_short_form);

  // 原代码的 `& 0x03FF` 是一个安全措施，确保结果在-999到999之间。
  // 在严格遵守1BRC输入格式的前提下，此步骤可以省略，但保留更安全。
  return {result & 0x03FF, n};
}
