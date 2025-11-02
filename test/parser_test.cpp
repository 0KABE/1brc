//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <gtest/gtest.h>

TEST(Parser, BasicParseOnce) {
  using namespace std::string_view_literals;

  // Hamburg;12.0
  // Bulawayo;8.9
  // Palembang;38.8
  // Hamburg;34.2
  // St. John's;15.2
  // Cracow;12.6
  std::vector<std::tuple<std::span<const char>, Entity>> v{
      {"Hamburg;12.0"sv, {"Hamburg", 120}},        //
      {"Bulawayo;8.9"sv, {"Bulawayo", 89}},        //
      {"Palembang;38.8"sv, {"Palembang", 388}},    //
      {"Hamburg;34.2"sv, {"Hamburg", 342}},        //
      {"St. John's;15.2"sv, {"St. John's", 152}},  //
      {"Cracow;12.6"sv, {"Cracow", 126}},          //
  };

  for (auto [span, expect] : v) {
    auto actual = BasicParseOnce(span);
    EXPECT_EQ(actual, expect);
    EXPECT_TRUE(span.empty());
  }
}