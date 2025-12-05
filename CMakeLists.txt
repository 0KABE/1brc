cmake_minimum_required(VERSION 3.30)

project(1brc)

find_package(GTest)
find_package(benchmark)
find_package(fmt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-march=native -Wall)

# Core Lib
add_library(${PROJECT_NAME}_core
        src/single_line_reader.cpp
        src/multi_line_reader.cpp
        src/number_reader.cpp
)
target_include_directories(${PROJECT_NAME}_core PUBLIC src)
target_link_libraries(${PROJECT_NAME}_core PUBLIC fmt::fmt)

# Executor
add_executable(${PROJECT_NAME}_main src/main.cpp)
target_link_libraries(${PROJECT_NAME}_main ${PROJECT_NAME}_core benchmark::benchmark_main)

# Test Code
add_executable(${PROJECT_NAME}_test
        test/main.cpp
        test/parser_test.cpp
        test/multi_line_reader_test.cpp
)
target_include_directories(${PROJECT_NAME}_test PRIVATE src)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_core GTest::gtest)

# Benchmark
add_executable(${PROJECT_NAME}_benchmark
        benchmark/basic_benchmark.cpp
        benchmark/parser_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark ${PROJECT_NAME}_core benchmark::benchmark_main)
