//
// Created by Chen, WenTao on 2025/11/3.
//

#include "parser.h"

#include <gtest/gtest.h>

TEST(Parser, BasicParseOnce) {
  using namespace std::string_view_literals;

  std::vector<std::tuple<std::span<const char>, Entity>> v{
      {"Dampier;34.3\n"sv, {"Dampier", 343}},            //
      {"Kansas City;19.5\n"sv, {"Kansas City", 195}},    //
      {"Benghazi;16.1\n"sv, {"Benghazi", 161}},          //
      {"Portland (OR);8.8\n"sv, {"Portland (OR)", 88}},  //
      {"Sokoto;17.8\n"sv, {"Sokoto", 178}},              //
      {"Valletta;22.6\n"sv, {"Valletta", 226}},          //
      {"Tokyo;15.0\n"sv, {"Tokyo", 150}},                //
      {"Accra;30.0\n"sv, {"Accra", 300}},                //
      {"Upington;36.3\n"sv, {"Upington", 363}},          //
      {"Entebbe;23.8\n"sv, {"Entebbe", 238}},            //
      {"Ljubljana;16.6\n"sv, {"Ljubljana", 166}},        //
      {"New Orleans;8.3\n"sv, {"New Orleans", 83}},      //
      {"Cracow;-1.7\n"sv, {"Cracow", -17}},              //
      {"Brisbane;6.6\n"sv, {"Brisbane", 66}},            //
      {"Hanoi;8.5\n"sv, {"Hanoi", 85}},                  //
      {"Alexandra;8.0\n"sv, {"Alexandra", 80}},          //
      {"Anadyr;-2.8\n"sv, {"Anadyr", -28}},              //
      {"Denver;1.4\n"sv, {"Denver", 14}},                //
      {"Blantyre;27.5\n"sv, {"Blantyre", 275}},          //
      {"Singapore;53.3\n"sv, {"Singapore", 533}},        //
      {"Thiès;33.4\n"sv, {"Thiès", 334}},                //
      {"Hamburg;12.0\n"sv, {"Hamburg", 120}},            //
      {"Bulawayo;8.9\n"sv, {"Bulawayo", 89}},            //
      {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon;8.9\n"sv,
       {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon",
        89}},  //
  };

  for (auto [span, expect] : v) {
    const auto [name, temperature] = ParseOnce_Base(span);
    EXPECT_EQ(name, expect.name);
    EXPECT_EQ(temperature, expect.temperature);
    EXPECT_TRUE(span.empty());
  }
}

// TEST(Parser, ParseOnceV1) {
//   using namespace std::string_view_literals;
//
//   std::vector<std::tuple<std::span<const char>, Entity>> v{
//       {"Dampier;34.3\n"sv, {"Dampier", 343}},            //
//       {"Kansas City;19.5\n"sv, {"Kansas City", 195}},    //
//       {"Benghazi;16.1\n"sv, {"Benghazi", 161}},          //
//       {"Portland (OR);8.8\n"sv, {"Portland (OR)", 88}},  //
//       {"Sokoto;17.8\n"sv, {"Sokoto", 178}},              //
//       {"Valletta;22.6\n"sv, {"Valletta", 226}},          //
//       {"Tokyo;15.0\n"sv, {"Tokyo", 150}},                //
//       {"Accra;30.0\n"sv, {"Accra", 300}},                //
//       {"Upington;36.3\n"sv, {"Upington", 363}},          //
//       {"Entebbe;23.8\n"sv, {"Entebbe", 238}},            //
//       {"Ljubljana;16.6\n"sv, {"Ljubljana", 166}},        //
//       {"New Orleans;8.3\n"sv, {"New Orleans", 83}},      //
//       {"Cracow;-1.7\n"sv, {"Cracow", -17}},              //
//       {"Brisbane;6.6\n"sv, {"Brisbane", 66}},            //
//       {"Hanoi;8.5\n"sv, {"Hanoi", 85}},                  //
//       {"Alexandra;8.0\n"sv, {"Alexandra", 80}},          //
//       {"Anadyr;-2.8\n"sv, {"Anadyr", -28}},              //
//       {"Denver;1.4\n"sv, {"Denver", 14}},                //
//       {"Blantyre;27.5\n"sv, {"Blantyre", 275}},          //
//       {"Singapore;53.3\n"sv, {"Singapore", 533}},        //
//       {"Thiès;33.4\n"sv, {"Thiès", 334}},                //
//       {"Hamburg;12.0\n"sv, {"Hamburg", 120}},            //
//       {"Bulawayo;8.9\n"sv, {"Bulawayo", 89}},            //
//       {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon;8.9\n"sv,
//        {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon",
//         89}},  //
//   };
//
//   for (auto [span, expect] : v) {
//     const auto [name, temperature] = ParseOnceV1(span);
//     EXPECT_EQ(name, expect.name);
//     EXPECT_EQ(temperature, expect.temperature);
//     EXPECT_TRUE(span.empty());
//   }
// }

TEST(Bits, FindFirstZeroByte) {
  EXPECT_EQ(FindFirstZeroByte(0x1234567812345678), 9);
  EXPECT_EQ(FindFirstZeroByte(0x1234007812345678), 6);
  EXPECT_EQ(FindFirstZeroByte(0x9ABCDEFF00005678), 3);
}