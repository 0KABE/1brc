//
// Created by Chen, WenTao on 2025/11/3.
//

#include <gtest/gtest.h>

#include "single_line_reader.h"

template <typename>
class ParserTest : public ::testing::Test {};

using ParserImplementations = ::testing::Types<
    SingleLineReader_Base,                               //
    SingleLineReader<FindByte_Base, NumberReader_Base>,  //
    SingleLineReader<FindByte_Base, NumberReader_SWAR_V1>, SingleLineReader<FindByte_Base, NumberReader_SWAR_V2>,
    SingleLineReader<FindByte_SWAR, NumberReader_Base>, SingleLineReader<FindByte_SWAR, NumberReader_SWAR_V1>,
    SingleLineReader<FindByte_SWAR, NumberReader_SWAR_V2>>;

TYPED_TEST_SUITE(ParserTest, ParserImplementations);

TYPED_TEST(ParserTest, AllParsers) {
  using namespace std::string_view_literals;

  std::vector<std::tuple<std::span<const char>, Entity>> v{
      {"Dampier;34.3\n"sv, {"Dampier"sv, 343}},            //
      {"Kansas City;19.5\n"sv, {"Kansas City"sv, 195}},    //
      {"Benghazi;16.1\n"sv, {"Benghazi"sv, 161}},          //
      {"Portland (OR);8.8\n"sv, {"Portland (OR)"sv, 88}},  //
      {"Sokoto;17.8\n"sv, {"Sokoto"sv, 178}},              //
      {"Valletta;22.6\n"sv, {"Valletta"sv, 226}},          //
      {"Tokyo;15.0\n"sv, {"Tokyo"sv, 150}},                //
      {"Accra;30.0\n"sv, {"Accra"sv, 300}},                //
      {"Upington;36.3\n"sv, {"Upington"sv, 363}},          //
      {"Entebbe;23.8\n"sv, {"Entebbe"sv, 238}},            //
      {"Ljubljana;16.6\n"sv, {"Ljubljana"sv, 166}},        //
      {"New Orleans;8.3\n"sv, {"New Orleans"sv, 83}},      //
      {"Cracow;-1.7\n"sv, {"Cracow"sv, -17}},              //
      {"Brisbane;6.6\n"sv, {"Brisbane"sv, 66}},            //
      {"Hanoi;8.5\n"sv, {"Hanoi"sv, 85}},                  //
      {"Alexandra;8.0\n"sv, {"Alexandra"sv, 80}},          //
      {"Anadyr;-2.8\n"sv, {"Anadyr"sv, -28}},              //
      {"Denver;1.4\n"sv, {"Denver"sv, 14}},                //
      {"Blantyre;27.5\n"sv, {"Blantyre"sv, 275}},          //
      {"Singapore;53.3\n"sv, {"Singapore"sv, 533}},        //
      {"Thiès;33.4\n"sv, {"Thiès"sv, 334}},                //
      {"Hamburg;12.0\n"sv, {"Hamburg"sv, 120}},            //
      {"Bulawayo;8.9\n"sv, {"Bulawayo"sv, 89}},            //
      {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon;8.9\n"sv,
       {"ASDGABASDGWTERYdfghonpn apsndonpgnpljkjlqwet7723lj =-=zxcvasd,.//sdaf!@#$%^&*()<>TERYdfghonpn apsndon"sv,
        89}},  //
  };

  for (auto [span, expect] : v) {
    const auto [name, temperature] = TypeParam{}(span);
    EXPECT_EQ(name, expect.name);
    EXPECT_EQ(temperature, expect.temperature);
    EXPECT_TRUE(span.empty());
  }
}

TEST(Bits, FindFirstZeroByte) {
  EXPECT_EQ(FindLowestZeroByte_SWAR{}(0x1234567812345678), 8);
  EXPECT_EQ(FindLowestZeroByte_SWAR{}(0x1234007812345678), 5);
  EXPECT_EQ(FindLowestZeroByte_SWAR{}(0x9ABCDEFF00005678), 2);
  EXPECT_EQ(FindLowestZeroByte_SWAR{}(0x9ABCDEFF01005678), 2);
  EXPECT_EQ(FindLowestZeroByte_SWAR{}(0x9ABCDEFF00015678), 3);

  EXPECT_EQ(FindLowestZeroByte_Base{}(0x1234567812345678), 8);
  EXPECT_EQ(FindLowestZeroByte_Base{}(0x1234007812345678), 5);
  EXPECT_EQ(FindLowestZeroByte_Base{}(0x9ABCDEFF00005678), 2);
  EXPECT_EQ(FindLowestZeroByte_Base{}(0x9ABCDEFF01005678), 2);
  EXPECT_EQ(FindLowestZeroByte_Base{}(0x9ABCDEFF00015678), 3);
}

TEST(Bits, ParseNumber) {
  using Result = NumberReaderResult;
  EXPECT_EQ(NumberReader_Base{}("12.1"), Result(4, 121));
  EXPECT_EQ(NumberReader_Base{}("-12.1"), Result(5, -121));
  EXPECT_EQ(NumberReader_Base{}("0.1"), Result(3, 1));
  EXPECT_EQ(NumberReader_Base{}("-0.1"), Result(4, -1));
  EXPECT_EQ(NumberReader_Base{}("99.9"), Result(4, 999));
  EXPECT_EQ(NumberReader_Base{}("-99.9"), Result(5, -999));

  EXPECT_EQ(NumberReader_SWAR_V1{}("12.1"), Result(4, 121));
  EXPECT_EQ(NumberReader_SWAR_V1{}("-12.1"), Result(5, -121));
  EXPECT_EQ(NumberReader_SWAR_V1{}("0.1"), Result(3, 1));
  EXPECT_EQ(NumberReader_SWAR_V1{}("-0.1"), Result(4, -1));
  EXPECT_EQ(NumberReader_SWAR_V1{}("99.9"), Result(4, 999));
  EXPECT_EQ(NumberReader_SWAR_V1{}("-99.9"), Result(5, -999));

  EXPECT_EQ(NumberReader_SWAR_V2{}("12.1"), Result(4, 121));
  EXPECT_EQ(NumberReader_SWAR_V2{}("-12.1"), Result(5, -121));
  EXPECT_EQ(NumberReader_SWAR_V2{}("0.1"), Result(3, 1));
  EXPECT_EQ(NumberReader_SWAR_V2{}("-0.1"), Result(4, -1));
  EXPECT_EQ(NumberReader_SWAR_V2{}("99.9"), Result(4, 999));
  EXPECT_EQ(NumberReader_SWAR_V2{}("-99.9"), Result(5, -999));
}
