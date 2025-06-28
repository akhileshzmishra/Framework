//
// Created by Akhilesh Mishra on 14/12/2024.
//

#ifndef TESTHEADER_HPP
#define TESTHEADER_HPP

#include <benchmark/benchmark.h>
#include <TestHeader.hpp>
#include <InPlaceStack.hpp>
#include <iostream>
#include <thread>
#include <bitset>
#include <SPSCQueue.hpp>

namespace testSpace
{
    class Test: public benchmark::Fixture{
    public:
        static void benchMarkCache(::benchmark::State& state);
        static void testInplaceSPSCQueue(::benchmark::State& state);
        static void testInplaceMPMCCQueue(::benchmark::State& state);
        void SetUp(::benchmark::State& state) override;

        void TearDown(::benchmark::State& state) override;
    };


};


#endif //TESTHEADER_HPP
