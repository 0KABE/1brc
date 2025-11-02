cmake_minimum_required(VERSION 3.30)

project(1brc)

find_package(GTest)
find_package(benchmark)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-march=native)

add_executable(${PROJECT_NAME}_main src/main.cpp)
target_include_directories(${PROJECT_NAME}_main PRIVATE src)
target_link_libraries(${PROJECT_NAME}_main benchmark::benchmark_main)

add_executable(${PROJECT_NAME}_test
        test/main.cpp
        test/parser_test.cpp
)
target_include_directories(${PROJECT_NAME}_test PRIVATE src)
target_link_libraries(${PROJECT_NAME}_test GTest::gtest)

add_executable(${PROJECT_NAME}_benchmark
        benchmark/main.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark benchmark::benchmark_main)
