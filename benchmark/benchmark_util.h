//
// Created by Chen, WenTao on 2025/12/19.
//

#pragma once

#include <fmt/format.h>

#include <random>
#include <string>

inline std::string RandomStation() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution name_length_dist(1, 100);
  std::uniform_int_distribution<> char_dist('a', 'z');

  std::string name;
  const auto length = name_length_dist(gen);
  for (int i = 0; i < length; ++i) {
    name += static_cast<char>(char_dist(gen));
  }
  return name;
}

inline std::string RandomTemperature() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution negative_dist(0, 1);
  std::uniform_int_distribution digit_dist(0, 999);

  const auto n = digit_dist(gen);
  return fmt::format("{}{}.{}\n", negative_dist(gen) ? "-" : "", n / 10, n % 10);
}
