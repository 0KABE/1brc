cmake_minimum_required(VERSION 3.30)

project(1brc)

find_package(GTest)
find_package(benchmark)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-march=native)

add_executable(${PROJECT_NAME}_parse_station_v2 src/parse_station_v2_main.cpp
        src/env.h
        src/parse.h
        src/structs.h
        src/file_memory_map.h)
target_link_libraries(${PROJECT_NAME}_parse_station_v2 benchmark::benchmark_main)

add_executable(${PROJECT_NAME}_parse_station_v3 src/parse_station_v3_main.cpp
        src/env.h
        src/parse.h
        src/structs.h
        src/file_memory_map.h)
target_link_libraries(${PROJECT_NAME}_parse_station_v3 benchmark::benchmark_main)

add_executable(benchmark
        src/env.h
        benchmark/parse_number_test.cpp)
target_link_libraries(benchmark benchmark::benchmark_main)
target_include_directories(benchmark PRIVATE src)
