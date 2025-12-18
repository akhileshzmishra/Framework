cmake_minimum_required(VERSION 3.6)
project(akhil-project)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fsanitize=undefined -O3")
set(BENCHMARK_DOWNLOAD_DEPENDENCIES, "ON")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(BENCHMARK_DOWNLOAD_DEPENDENCIES, "ON")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/ax")
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/ax")
set(CMAKE_GENERATOR "Ninja")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

include_directories(
        ./inplace
        ./benchmark/include
        ./googletest/include
)
add_subdirectory(googletest)
add_subdirectory(benchmark)
add_subdirectory(inplace)
add_subdirectory(datastructure)
add_subdirectory(service)
add_subdirectory(unit)