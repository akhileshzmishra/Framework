set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_INIT "-fsanitize=undefined")

enable_testing()

add_executable(
        unittest_ubsan
        ../../SPSCQueuetest.cpp
        ../../MPMCQueueTest.cpp
        ../../MultiKeyHashMapTest.cpp
)

#find_package(benchmark REQUIRED)
target_include_directories(
        unittest_ubsan PUBLIC
        ../../.
        ../../../inplace
        ../../../datastructure
        ../../../benchmark/include
        ../../../googletest/googletest/include/
)

target_link_libraries(
        unittest_ubsan
        inplace
        gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unittest_ubsan)
