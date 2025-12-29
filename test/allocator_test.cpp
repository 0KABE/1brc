//
// Created by Chen, WenTao on 2025/12/6.
//

#include "allocator.h"

#include <gtest/gtest.h>

#include <ranges>

TEST(Allocator, Base) {
  std::vector<std::string_view> raw_input = {
      "AAA;-1.0",  //
      "BBB;99.9",  //
      "CCC;0.0",   //
      "AAA;1.0",   //
      "AAA;2.0",   //
      "BBB;99.9",  //
  };

  auto input = raw_input                                                                      //
               | std::ranges::views::transform([](auto s) { return std::string(s) + "\n"; })  //
               | std::views::join                                                             //
               | std::ranges::to<std::string>();

  Allocator allocator(input);

  EXPECT_EQ(allocator.Allocate(1).size(), 9);
  EXPECT_EQ(allocator.Allocate(9).size(), 17);
}