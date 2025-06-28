#include <TestHeader.hpp>
#include <InPlaceStack.hpp>
#include <iostream>
#include <thread>
#include <bitset>
#include <SPSCQueue.hpp>
#include <benchmark/benchmark.h>

#include "MPMCQueue.hpp"

using namespace testSpace;

void Test::SetUp (::benchmark::State& state) {
    (void)state;
}

void Test::TearDown (::benchmark::State& state) {
    (void)state;
}

void Test::benchMarkCache (::benchmark::State& state) {
    const size_t size = state.range (0);

    while (state.KeepRunning ()) {
        std::vector<int64_t> tempVector (size, 0);
        const size_t K = 256;

        for (size_t i = 0; i < K; i++) {
            for (size_t j = 0; j < size; j++) {
                tempVector[j]++;
            }
        }
    }
}

void Test::testInplaceSPSCQueue (::benchmark::State& state) {
    const size_t size = state.range (0);

    while (state.KeepRunning ()) {
        inplace::SPSCQueue<long, 1 << 5> spscQueue;
        std::vector<int64_t> writeVector;
        std::vector<int64_t> readVector;
        std::thread writer ([&spscQueue, &writeVector, size] {
            for (size_t i = 0; i < size; i++) {
                while (spscQueue.full ()) {
                    //std::cout << "Writer = null" << std::endl;
                    continue;
                }
                int num = i + 21;
                if (!spscQueue.enqueue (num)) {
                    //std::cerr << "Enqueue failed" << std::endl;
                }
                //std::cout << "Writer = " << num << std::endl;
                writeVector.push_back(num);
            }
        });

        std::thread reader ([&spscQueue, &readVector, size] {
            for (size_t i = 0; i < size; i++) {
                while (spscQueue.empty ()) {
                    //std::cout << "Reader = null" << std::endl;
                    continue;
                }
                if (!spscQueue.dequeue ([&readVector](const long& input) {
                    //read[input] = true;
                    //std::cout << "Reader = " << input << std::endl;
                    readVector.push_back (input);
                })) {
                    //std::cout << "Reader failed to dequeue" << std::endl;
                }

            }
        });
        writer.join ();
        reader.join ();

        if (writeVector.size() != readVector.size()) {
            //std::cout<<"Writer wrote "<<writeVector.size()<<" & Reader read "<<writeVector.size()<<std::endl;
        }
        else {
            //std::cout<<"Consumed "<<writeVector.size()<<std::endl;
        }
        auto writerSize = writeVector.size();
        auto readerSize = readVector.size();
        size_t writeLooped = 0;
        while ((writeLooped < writerSize) && (writeLooped < readerSize)) {
            if (readVector[writeLooped] != writeVector[writeLooped]) {
                //std::cout<<"Read="<<readVector[writeLooped]<<"' " << "Write="<<writeVector[writeLooped]<<std::endl;
            }
            else {
                //std::cout<<"Wrote & Read="<<readVector[writeLooped]<<std::endl;;
            }
            writeLooped++;
        }
        while ((writeLooped < writerSize)) {
            //std::cout<<"Write Unread="<<writeVector[writeLooped]<<std::endl;
            writeLooped++;
        }
        while ((writeLooped < readerSize)) {
            //std::cout<<"Read garbage="<<readVector[writeLooped]<<std::endl;;
            writeLooped++;
        }

    }
    std::cout << std::endl;
}


void Test::testInplaceMPMCCQueue(::benchmark::State& state) {
    const size_t size = state.range (0);

    while (state.KeepRunning ()) {
        inplace::MPMCQueue<long, 1 << 5> mpmcQueue;
        std::vector<int64_t> writeVector;
        std::vector<int64_t> readVector;
        std::thread writer ([&mpmcQueue, &writeVector, size] {
            for (size_t i = 0; i < size; i++) {
                while (mpmcQueue.full ()) {
                    //std::cout << "Writer = null" << std::endl;
                    continue;
                }
                int num = i + 21;
                if (!mpmcQueue.enqueue (num)) {
                    //std::cerr << "Enqueue failed" << std::endl;
                }
                //std::cout << "Writer = " << num << std::endl;
                writeVector.push_back(num);
            }
        });

        std::thread reader ([&mpmcQueue, &readVector, size] {
            for (size_t i = 0; i < size; i++) {
                while (mpmcQueue.empty ()) {
                    //std::cout << "Reader = null" << std::endl;
                    continue;
                }
                if (!mpmcQueue.dequeue ([&readVector](const long& input) {
                    //read[input] = true;
                    //std::cout << "Reader = " << input << std::endl;
                    readVector.push_back (input);
                })) {
                    //std::cout << "Reader failed to dequeue" << std::endl;
                }

            }
        });
        writer.join ();
        reader.join ();

        if (writeVector.size() != readVector.size()) {
            std::cout<<"Writer wrote "<<writeVector.size()<<" & Reader read "<<writeVector.size()<<std::endl;
        }
        else {
            //std::cout<<"Consumed "<<writeVector.size()<<std::endl;
        }
        auto writerSize = writeVector.size();
        auto readerSize = readVector.size();
        size_t writeLooped = 0;
        while ((writeLooped < writerSize) && (writeLooped < readerSize)) {
            if (readVector[writeLooped] != writeVector[writeLooped]) {
                std::cout<<"Read="<<readVector[writeLooped]<<"' " << "Write="<<writeVector[writeLooped]<<std::endl;
            }
            else {
                //std::cout<<"Wrote & Read="<<readVector[writeLooped]<<std::endl;;
            }
            writeLooped++;
        }
        while ((writeLooped < writerSize)) {
            std::cout<<"Write Unread="<<writeVector[writeLooped]<<std::endl;
            writeLooped++;
        }
        while ((writeLooped < readerSize)) {
            std::cout<<"Read garbage="<<readVector[writeLooped]<<std::endl;;
            writeLooped++;
        }
    }
}