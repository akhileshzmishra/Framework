set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(sanitizers)

enable_testing()

add_executable(
        unittest
        SPSCQueuetest.cpp
        MPMCQueueTest.cpp
        MultiKeyHashMapTest.cpp
        SudokuSolverTest.cpp
        MyFunctionTest.cpp
        MyDatastructureTest.cpp
        ZipViewTest.cpp
)

#find_package(benchmark REQUIRED)
target_include_directories(
        unittest PUBLIC
        .
        ../inplace
        ../datastructure
        ../benchmark/include
        ../googletest/googletest/include/
)

target_link_libraries(
        unittest
        inplace
        gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unittest)
