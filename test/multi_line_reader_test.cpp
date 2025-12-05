//
// Created by Chen, WenTao on 2025/12/5.
//

#include "multi_line_reader.h"

#include <gtest/gtest.h>

#include <ranges>

TEST(MultiLineReader, Base) {
  std::vector<std::string_view> raw_input = {
      "AAA;-1.0",  //
      "BBB;99.9",  //
      "CCC;0.0",   //
      "AAA;1.0",   //
      "AAA;2.0",   //
      "BBB;99.9",  //
  };

  auto input = raw_input                                                              //
               | std::views::transform([](auto s) { return std::string(s) + "\n"; })  //
               | std::views::join                                                     //
               | std::ranges::to<std::string>();

  MultiLineReader reader;
  reader.Parse(input);
  EXPECT_EQ(reader.GetStatistics().size(), 3);
  EXPECT_EQ(reader.GetStatistics().at("AAA").min, -10);
  EXPECT_EQ(reader.GetStatistics().at("AAA").max, 20);
  EXPECT_EQ(reader.GetStatistics().at("AAA").count, 3);
  EXPECT_EQ(reader.GetStatistics().at("AAA").sum, 20);
  EXPECT_EQ(reader.GetStatistics().at("BBB").min, 999);
  EXPECT_EQ(reader.GetStatistics().at("BBB").max, 999);
  EXPECT_EQ(reader.GetStatistics().at("BBB").count, 2);
  EXPECT_EQ(reader.GetStatistics().at("BBB").sum, 1998);
}