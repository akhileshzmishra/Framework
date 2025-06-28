add_executable(
        tester
        InplaceListTester.cpp
        BenchmarkProject.cpp
)

set(CMAKE_CXX_FLAGS_INIT "-fsanitize=undefined")

#find_package(benchmark REQUIRED)
target_include_directories(
        tester PUBLIC
        .
        ../inplace
        ../benchmark/include
)

link_libraries(
        tester
        inplace
)


target_link_libraries(tester benchmark::benchmark)