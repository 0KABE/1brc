cmake_minimum_required(VERSION 3.30)

project(1brc)

find_package(GTest)

add_executable(${PROJECT_NAME} src/main.cpp)

add_executable(benchmark benchmark/main.cpp)

target_link_libraries(benchmark gtest::gtest)