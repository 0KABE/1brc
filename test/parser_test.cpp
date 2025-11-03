//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <gtest/gtest.h>

TEST(Parser, BasicParseOnce) {
  using namespace std::string_view_literals;

  std::vector<std::tuple<std::span<const char>, Entity>> v{
      {"Dampier;34.3"sv, {"Dampier", 343}},            //
      {"Kansas City;19.5"sv, {"Kansas City", 195}},    //
      {"Benghazi;16.1"sv, {"Benghazi", 161}},          //
      {"Portland (OR);8.8"sv, {"Portland (OR)", 88}},  //
      {"Sokoto;17.8"sv, {"Sokoto", 178}},              //
      {"Valletta;22.6"sv, {"Valletta", 226}},          //
      {"Tokyo;15.0"sv, {"Tokyo", 150}},                //
      {"Accra;30.0"sv, {"Accra", 300}},                //
      {"Upington;36.3"sv, {"Upington", 363}},          //
      {"Entebbe;23.8"sv, {"Entebbe", 238}},            //
      {"Ljubljana;16.6"sv, {"Ljubljana", 166}},        //
      {"New Orleans;8.3"sv, {"New Orleans", 83}},      //
      {"Cracow;-1.7"sv, {"Cracow", -17}},              //
      {"Brisbane;6.6"sv, {"Brisbane", 66}},            //
      {"Hanoi;8.5"sv, {"Hanoi", 85}},                  //
      {"Alexandra;8.0"sv, {"Alexandra", 80}},          //
      {"Anadyr;-2.8"sv, {"Anadyr", -28}},              //
      {"Denver;1.4"sv, {"Denver", 14}},                //
      {"Blantyre;27.5"sv, {"Blantyre", 275}},          //
      {"Singapore;53.3"sv, {"Singapore", 533}},        //
      {"Thiès;33.4"sv, {"Thiès", 334}},                //
      {"Hamburg;12.0"sv, {"Hamburg", 120}},            //
      {"Bulawayo;8.9"sv, {"Bulawayo", 89}},            //
  };

  for (auto [span, expect] : v) {
    const auto [name, temperature] = BasicParseOnce(span);
    EXPECT_EQ(name, expect.name);
    EXPECT_EQ(temperature, expect.temperature);
    EXPECT_TRUE(span.empty());
  }
}