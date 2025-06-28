//
// Created by Akhilesh Mishra on 15/12/2024.
//

#include <TestHeader.hpp>
#include <benchmark/benchmark.h>
#include <iostream>
using namespace testSpace;
using namespace std;
using namespace benchmark;
//BENCHMARK(testSpace::Test::testInplaceList);
//BENCHMARK(testSpace::Test::benchMarkCache)->RangeMultiplier(2)->Range(1, 1 << 20);
//BENCHMARK(testSpace::Test::testInplaceSPSCQueue)->Repetitions (10)->RangeMultiplier(2)->Range(1, 1 << 20);
BENCHMARK(testSpace::Test::testInplaceMPMCCQueue)->Repetitions (10)->RangeMultiplier(2)->Range(1, 1<< 20);
BENCHMARK_MAIN();