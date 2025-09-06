//
// Created by Chen, WenTao on 2025/8/19.
//

#pragma once

#include "structs.h"

#include <climits>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <span>
#include <string_view>
#include <thread>

constexpr uint64_t get_delimiter_mask(char c) {
  uint64_t v = 0;
  for (int i = 0; i < sizeof(uint64_t); ++i) {
    v = v << CHAR_BIT | c;
  }
  return v;
}

constexpr uint64_t filter_zero_bytes(uint64_t v) {
  v = v - 0x0101010101010101 & ~v & 0x8080808080808080;
  return v;
}

int parse_number_v1(const std::span<const char> span) {
  const uint64_t v = *reinterpret_cast<const uint64_t *>(span.data());
  constexpr auto sign_mask = get_delimiter_mask('-');
  const int negative = (static_cast<int>(v ^ sign_mask) & 0xFF) - 1;
  const uint64_t pure_number = (v >> 8 * negative) & 0x0F000F0F;
  const int number = (pure_number & 0x0F) * 100 +
                     ((pure_number & 0x0F00) >> 8) * 10 +
                     ((pure_number & 0x0F000000) >> 8 * 3);

  return negative ? -number : number;
}

std::tuple<Temperature, int> parse_number_v2(const std::span<const char> span) {
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

std::tuple<Temperature, int> parse_number_v3(const std::span<const char> span) {
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
  const int result =
      static_cast<int>((number ^ is_negative_mask) - is_negative_mask);

  // 9. 计算消耗的字符数。
  //    基础长度是 "dd.d" (4个字符)。
  //    负号多一个字符，is_negative_mask为-1, -(-1) = +1。
  //    "d.d"格式少一个字符, is_short_form为1, -1。
  const int n = 4 + static_cast<int>(-is_negative_mask - is_short_form);

  // 原代码的 `& 0x03FF` 是一个安全措施，确保结果在-999到999之间。
  // 在严格遵守1BRC输入格式的前提下，此步骤可以省略，但保留更安全。
  return {result & 0x03FF, n};
}

inline std::tuple<std::string_view, Temperature>
ParseStation_V1(const char *&ptr) {
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

std::tuple<std::string_view, Temperature> ParseStation_V2(const char *&ptr) {
  std::string_view name;
  int temperature;

  // --- 1. 使用AVX2向量化查找分号';' ---
  {
    // 创建一个所有字节都是';'的32字节向量
    const __m256i semicolon_vec = _mm256_set1_epi8(';');

    // 从当前指针位置加载32字节数据（非对齐加载）
    const __m256i chunk =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(ptr));

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
    p += (*p == '-'); // 如果是负号，指针前进一位

    // 一次性读取最多4个字节（例如 "12.3" 或 "9.8"）
    // memcpy可以避免非对齐访问问题，且通常会被编译器优化为一次读取
    uint32_t four_bytes;
    std::memcpy(&four_bytes, p, sizeof(four_bytes));

    // 小数点的位置决定了数字的格式
    // 检查第二个字节是否是'.' (e.g., "9.8")
    const int dot_pos = (four_bytes >> 8) & 0xFF;

    if (dot_pos == '.') { // 格式: d.d (例如 9.8)
      // 提取个位数和十分位数
      const int d1 = (four_bytes & 0xFF) - '0';
      const int d2 = (four_bytes >> 16) & 0xFF - '0';
      temperature = (d1 * 10 + d2) * sign_multiplier;
      ptr = p + 3; // 跳过 "d.d"
    } else {       // 格式: dd.d (例如 12.3)
      // 提取十位数、个位数和十分位数
      const int d1 = (four_bytes & 0xFF) - '0';
      const int d2 = (four_bytes >> 8) & 0xFF - '0';
      const int d3 = (four_bytes >> 24) & 0xFF - '0';
      temperature = (d1 * 100 + d2 * 10 + d3) * sign_multiplier;
      ptr = p + 4; // 跳过 "dd.d"
    }
    ptr++; // 跳过最后的换行符 '\n'
  }

  return {name, temperature};
}

inline int AVX256Find(const std::span<const char> span, const char target) {
  const __m256i semicolon_vec = _mm256_set1_epi8(target);
  const __m256i chunk =
      _mm256_loadu_si256(reinterpret_cast<const __m256i *>(span.data()));
  const __m256i comparison_mask = _mm256_cmpeq_epi8(chunk, semicolon_vec);
  const uint32_t mask = _mm256_movemask_epi8(comparison_mask);
  const int semicolon_pos = _tzcnt_u32(mask);
  return semicolon_pos;
}

inline std::tuple<std::string_view, Temperature>
ParseStation_V3(std::span<const char> &span) {
  // assume the station name is less than 32 bytes
  const int semicolon_pos = AVX256Find(span, ';');
  auto name = std::string_view(span.data(), semicolon_pos);
  span = span.subspan(semicolon_pos + 1);

  // const int new_line_pos = AVX256Find(span, '\n');
  auto [temperature, n] = parse_number_v2(span);
  span = span.subspan(n + 1);

  return {name, temperature};
}

struct HashMap {
  constexpr static size_t Size = 81920;
  using Key = std::string_view;
  struct Value {
    bool used{};
    Key key;
    StationStatistics value{};
  };
  using Index = size_t;
  using Bucket = std::array<Value, Size>;

  StationStatistics &operator[](const Key &key) {
    const auto hash = std::hash<Key>{}(key);
    const auto index = hash % Size;
    assert(index < Size);

    size_t pos = index;
    for (size_t i = 0; i < Size && bucket[pos].used;
         pos = (index + ++i) % Size) {
      if (bucket[pos].key == key) {
        return bucket[pos].value;
      }
    }

    if (!bucket[pos].used) {
      bucket[pos] = Value{true, key};
      return bucket[pos].value;
    }
    assert(false);
  }

  Bucket bucket;
};

void Merge(Statistics &dest, HashMap::Bucket &bucket) {
  static std::mutex mutex;
  std::lock_guard lock(mutex);
  for (auto &[used, key, value] : bucket) {
    if (!used) {
      continue;
    }

    if (const auto it = dest.find(key); it != dest.end()) {
      it->second.count += value.count;
      it->second.min = std::min(it->second.min, value.min);
      it->second.max = std::max(it->second.max, value.max);
      it->second.sum += value.sum;
    } else {
      dest[key] = value;
    }
  }
}

struct Worker {
  unsigned index;
  const FileMemoryMap &file;
  std::span<char> span;
  Statistics &all_statistics;
  std::thread thread;

  void Run() {
    HashMap statistics;
    auto [begin, end] = AlignSpan();
    while (begin < end) {
      auto [name, temperature] = ParseStation_V2(begin);

      auto &[count, min, max, sum] = statistics[name];
      max = std::max(max, temperature);
      min = std::min(min, temperature);
      sum += temperature;
      count++;
    }
    Merge(all_statistics, statistics.bucket);
  }

  [[nodiscard]] std::tuple<const char *, const char *> AlignSpan() const {
    const char *begin = &span.front();
    const char *end = &span.back();
    const char *file_end = static_cast<const char *>(file.address) + file.size;

    while (end < file_end && *end != '\n') {
      ++end;
    }
    while (begin < end && *begin != '\n') {
      ++begin;
    }
    ++begin;

    return {begin, end};
  }
};
