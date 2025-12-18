//
// Created by Akhilesh Mishra on 26/12/2024.
//

#include <gtest/gtest.h>

#include <CommonTestUtils.hpp>
#include <MPMCQueue.hpp>
#include <Signal.hpp>
#include <memory>
#include <string>
#include <type_traits>
using namespace inplace;


TEST (MPMCQueueTest, enqueue) {
    constexpr const size_t QSize = 35;
    inplace::MPMCQueue<DistortedStruct, QSize> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    for (size_t i = 0; i < QSize; i++) {
        vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
        q.enqueue (vecWriter.back ());
    }

    EXPECT_EQ (q.size (), QSize);
    EXPECT_EQ (q.full (), true);

    for (size_t i = 0; i < QSize; ++i) {
        auto elem = q.dequeue ([&vecReader](auto& x) {
                vecReader.push_back (x);
            });
        EXPECT_EQ (elem, true);
    }

    EXPECT_EQ (vecReader.size (), vecWriter.size ());
    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i));
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}

TEST (MPMCQueueTest, enqueueSizeOF1) {
    inplace::MPMCQueue<DistortedStruct, 1> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    for (size_t i = 0; i < 1; i++) {
        vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
        q.enqueue (vecWriter.back ());
    }

    EXPECT_EQ (q.size (), 1);
    EXPECT_EQ (q.full (), true);

    while (!q.empty ()) {
        auto elem = q.dequeue ([&vecReader](auto& x) {
            vecReader.push_back (x);
        });
        if (!elem) {
            EXPECT_EQ (elem, true);
        }
        EXPECT_EQ (elem, true);
    }

    EXPECT_EQ (vecReader.size (), vecWriter.size ());
    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i));
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}



TEST (MPMCQueueTest, waitTill) {
    inplace::MPMCQueue<DistortedStruct, 35> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    for (size_t i = 0; i < 36; i++) {
        vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
        bool expect = q.enqueue (vecWriter.back ());
        if (i == 35) {
            EXPECT_EQ (expect, false);
        } else {
            EXPECT_EQ (expect, true);
        }
    }

    while (!q.empty ()) {
        auto elem = q.dequeue ([&vecReader](auto& x) {
            vecReader.push_back (x);
        });
        EXPECT_EQ (elem, true);
    }

    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i));
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}

TEST (MPMCQueueTest, waitTillSizeOF1) {
    inplace::MPMCQueue<DistortedStruct, 1> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    for (size_t i = 0; i < 2; i++) {
        vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
        bool expect = q.enqueue (vecWriter.back ());
        if (i == 1) {
            EXPECT_EQ (expect, false);
        } else {
            EXPECT_EQ (expect, true);
        }
    }

    while (!q.empty ()) {
        auto elem = q.dequeue ([&vecReader](auto& x) {
            vecReader.push_back (x);
        });
        EXPECT_EQ (elem, true);
    }

    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i));
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}

TEST (MPMCQueueTest, threadedWrite) {
    constexpr const size_t QSize = 35;
    MPMCQueue<DistortedStruct, QSize> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    test::Signal s1, s2, s3;

    std::thread writer ([&vecWriter, &q, &s1, &s2, &s3, QSize] {
        for (size_t i = 0; i < QSize; i++) {
            auto obj = DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } };
            bool expect = q.enqueue (obj);
            EXPECT_EQ (expect, true);
            if (expect) {
               vecWriter.push_back (obj);
           }
        }

        EXPECT_EQ (q.size (), QSize);
        EXPECT_EQ (q.full (), true);

        for (size_t i = 0; i < 1; i++) {
            bool expect = q.enqueue (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
            EXPECT_EQ (expect, false);
            s1.signal ();
        }

        s2.waitFor();

        for (size_t i = 0; i < QSize; i++) {
            auto obj = DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } };

            bool expect = q.enqueue (obj);
            EXPECT_EQ (expect, true) << obj;
            if (expect) {
                vecWriter.push_back (obj);
            }
            else {
                std::cout << (q.full ()? "Q is full": "Q not full") << std::endl;
                std::cout << "Size of Q=" << q.size() << std::endl;
                q.print ();
            }
        }

        s3.signal ();

    });

    std::thread reader ([&vecReader, &q, &s1, &s2, &s3, QSize] {
        s1.waitFor();
        EXPECT_EQ (q.size(), QSize);
        EXPECT_EQ (q.full(), true);
        for (size_t i = 0; i < QSize; i++) {
            auto elem = q.dequeue ([&vecReader](auto& x) {
                vecReader.push_back (x);
            });
            EXPECT_EQ (elem, true);
        }

        for (size_t i = 0; i < 1; i++) {
            auto elem = q.dequeue ([&vecReader](const auto& x) {
                vecReader.push_back (x);
            });
            EXPECT_EQ (elem, false) << "at " << i;
            EXPECT_EQ (q.full (), false);
            EXPECT_EQ (q.empty (), true);
            if (elem) {
                std::cout << (q.empty ()? "Q is empty": "Q not empty") << std::endl;
                std::cout << "Size of Q=" << q.size() << std::endl;
                q.print ();
            }
        }
        s2.signal();
        s3.waitFor ();

        for (size_t i = 0; i < QSize; i++) {
            auto elem = q.dequeue ([&vecReader](const auto& x) {
                vecReader.push_back (x);
            });
            EXPECT_EQ (elem, true);
        }

    });
    writer.join ();
    reader.join ();

    EXPECT_EQ (vecReader.size (), vecWriter.size ());

    if (vecReader.size () != vecWriter.size ()) {
        std::cout << vecReader.size () << " != " << vecWriter.size () << std::endl;
        for (auto& read : vecReader) {
            std::cout << read << " ";
        }
        std::cout << std::endl;
        for (auto& write : vecWriter) {
            std::cout << write << " ";
        }
        std::cout << std::endl;
    }

    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i)) << vecReader.at (i) << ", " << vecWriter.at (i);
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}


TEST (MPMCQueueTest, SPSCTest) {
    constexpr size_t size = 1 << 20;

    inplace::MPMCQueue<DistortedStruct, 4> mpmcQueue;
    std::vector<DistortedStruct> writeVector;
    std::vector<DistortedStruct> readVector;
    std::thread writer ([&mpmcQueue, &writeVector] {
        for (size_t i = 0; i < size; i++) {
            auto obj = DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } };
            while (mpmcQueue.full ()) {
                //std::cout << "Writer = null" << std::endl;
                continue;
            }
            if (!mpmcQueue.enqueue (obj)) {
                //std::cerr << "Enqueue failed" << std::endl;
            }
            //std::cout << "Writer = " << num << std::endl;
            writeVector.push_back(obj);
        }
    });

    std::thread reader ([&mpmcQueue, &readVector] {
        for (size_t i = 0; i < size; i++) {
            while (mpmcQueue.empty ()) {
                //std::cout << "Reader = null" << std::endl;
                continue;
            }
            if (!mpmcQueue.dequeue ([&readVector](const auto& input) {
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
    EXPECT_EQ (writeVector.size (), readVector.size ());
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
    const auto minSize = std::min (readVector.size (), writeVector.size ());
    for (size_t i = 0; i < minSize; i++) {
        EXPECT_EQ (readVector.at (i), writeVector.at (i)) << readVector.at (i) << ", " << writeVector.at (i);
        EXPECT_EQ (readVector.at (i).d_ptr, writeVector.at (i).d_ptr);
    }
}

TEST (MPMCQueueTest, DISABLED_2P1CTest) {
    constexpr const size_t size = 1 << 15;

    inplace::MPMCQueue<DistortedStruct, 1 << 10, false> mpmcQueue;
    std::unordered_map<size_t, DistortedStruct> writeVector1;
    std::unordered_map<size_t, DistortedStruct> writeVector2;
    std::unordered_map<size_t, DistortedStruct> readVector;
    constexpr const size_t halfSize = size / 2;
    std::atomic<size_t> written = 0;
    std::thread writer1 ([&mpmcQueue, &writeVector1, &written] {
        for (size_t i = 0; i < halfSize; i++) {
            auto obj = DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } };
            WaitStrategy1 waitStrategy;
            while (!mpmcQueue.enqueue (obj) && mpmcQueue.isRunning ()) {
                (void)waitStrategy.wait ();
            }
            //std::cout << "Writer1 = " << obj << std::endl;
            written++;
            writeVector1[i] = obj;
        }
    });

    std::thread writer2 ([&mpmcQueue, &writeVector2, &written] {
        for (size_t i = halfSize; i < size; i++) {

            auto obj = DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } };
            WaitStrategy1 waitStrategy;
            while (!mpmcQueue.enqueue (obj)) {
                //std::cerr << "Enqueue2 failed" << std::endl;
                (void)waitStrategy.wait ();
            }
            //std::cout << "Writer2 = " << obj << std::endl;
            written++;
            writeVector2[i] = obj;
        }
    });

    std::thread reader ([&mpmcQueue, &readVector] {
        while (mpmcQueue.isRunning ()) {
            WaitStrategy1 waitStrategy;
            if (!mpmcQueue.dequeue ([&readVector](const auto& input) {
                //read[input] = true;
                //std::cout << "Reader = " << input << std::endl;
                readVector[input.x] = input;
            })) {
                //std::cout << "Reader failed to dequeue. size=" << mpmcQueue.size() << std::endl;
                //std::cout << "full=" << mpmcQueue.full() << std::endl;
                //std::cout << "written = " << written << std::endl;
                (void)waitStrategy.wait();
            }

        }
    });

    std::this_thread::sleep_for (std::chrono::milliseconds(20000));
    mpmcQueue.stop ();

    writer1.join ();
    writer2.join ();
    reader.join ();

    EXPECT_EQ (writeVector2.size () + writeVector1.size (), readVector.size ());
    if (writeVector2.size () + writeVector1.size () != readVector.size()) {
        std::cout<<"Writer wrote "<<writeVector2.size () + writeVector1.size ()<<" & Reader read "<<readVector.size()<<std::endl;
    }
    else {
        //std::cout<<"Consumed "<<writeVector.size()<<std::endl;
    }

    for (const auto& write : writeVector2) {
        EXPECT_EQ(readVector[write.first], write.second) << write.first << ", " << write.second;
    }

    for (const auto& write : writeVector1) {
        EXPECT_EQ(readVector[write.first], write.second) << write.first << ", " << write.second;;
    }

    for (const auto& read : readVector) {
        if (writeVector1[read.first] == read.second) {
            continue;
        }
        if (writeVector2[read.first] != read.second) {
            EXPECT_EQ(false, true) << read.first << ", " << read.second << " does not exist";
        }
    }
}

TEST (MPMCQueueTest, DISABLED_4P4CTest) {
    constexpr const size_t size = 1 << 15;
    int readerSize = 4;
    int writerSize = 4;
    inplace::MPMCQueue<DistortedStruct, 1 << 5, false> mpmcQueue;
    std::vector<DistortedStruct> ALLElements(size);
    std::vector<DistortedStruct> ReadElements(size);
    std::atomic<size_t> writeIndex = 0;

    std::vector<std::thread*> writers(writerSize);
    for (int i = 0; i < writerSize; i++) {
        writers[i] = new std::thread ([&writeIndex, &mpmcQueue, &ALLElements] () {
            auto index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            while (index < size) {
                auto obj = DistortedStruct{index, index, std::make_shared<int> (index), "abc",
                {index}};
                SpinLock<WaitStrategy1> spinlock;
                if (!mpmcQueue.enqueue (obj)) {
                    spinlock.GeneralWait ();
                    continue;
                }
                ALLElements[index] = obj;
                index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            }

        });
    }
    std::vector<std::thread*> readers(readerSize);
    for (int i = 0; i < readerSize; i++) {
        readers[i] = new std::thread ([&mpmcQueue, &ReadElements] () {

            while (mpmcQueue.isRunning ()) {
                //SpinLock<WaitStrategy1> spinlock;
                if (!mpmcQueue.dequeue ([&ReadElements](const auto& input) {
                    ReadElements[input.x] = input;
                })) {
                    //spinlock.GeneralWait ();
                    continue;
                }
            }
        });
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(30000));
    mpmcQueue.stop ();

    for (size_t i = 0; i < writers.size (); i++) {
        writers[i]->join ();
    }
    for (size_t i = 0; i < readers.size (); i++) {
        readers[i]->join ();
    }

    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ (ALLElements[i], ReadElements[i]);
    }
}

TEST (MPMCQueueTest, DISABLED_DISABLED_10P4CTest) {
    constexpr const size_t size = 1 << 20;
    int readerSize = 4;
    int writerSize = 10;
    inplace::MPMCQueue<DistortedStruct, 1 << 10, false> mpmcQueue;
    std::vector<DistortedStruct> ALLElements(size);
    std::vector<DistortedStruct> ReadElements(size);
    std::atomic<size_t> writeIndex = 0;

    std::vector<std::thread*> writers(writerSize);
    for (int i = 0; i < writerSize; i++) {
        writers[i] = new std::thread ([&writeIndex, &mpmcQueue, &ALLElements] () {
            auto index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            SpinLock<WaitStrategy1> spinlock;

            while (index < size) {
                auto obj = DistortedStruct{index, index, std::make_shared<int> (index), "abc",
                {index}};
                if (!mpmcQueue.enqueue (obj)) {
                    spinlock.GeneralWait ();
                    continue;
                }
                ALLElements[index] = obj;
                index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            }

        });
    }
    std::vector<std::thread*> readers(readerSize);
    for (int i = 0; i < readerSize; i++) {
        readers[i] = new std::thread ([&mpmcQueue, &ReadElements] () {

            while (mpmcQueue.isRunning ()) {
                //SpinLock<WaitStrategy1> spinlock;
                if (!mpmcQueue.dequeue ([&ReadElements](const auto& input) {
                    ReadElements[input.x] = input;
                })) {
                    //spinlock.GeneralWait ();
                    continue;
                }
            }
        });
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(30000));
    mpmcQueue.stop ();

    for (size_t i = 0; i < writers.size (); i++) {
        writers[i]->join ();
    }
    for (size_t i = 0; i < readers.size (); i++) {
        readers[i]->join ();
    }

    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ (ALLElements[i], ReadElements[i]);
    }
}

TEST (MPMCQueueTest, DISABLED_4P4CTestOnSmallQ) {
    constexpr const size_t size = 1 << 15;
    int readerSize = 1;
    int writerSize = 1;
    inplace::MPMCQueue<DistortedStruct, 2, false> mpmcQueue;
    std::vector<DistortedStruct> ALLElements(size);
    std::vector<DistortedStruct> ReadElements(size);
    std::atomic<size_t> writeIndex = 0;

    std::vector<std::thread*> writers(writerSize);
    for (int i = 0; i < writerSize; i++) {
        writers[i] = new std::thread ([&writeIndex, &mpmcQueue, &ALLElements] () {
            auto index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            while (index < size) {
                auto obj = DistortedStruct{index, index, std::make_shared<int> (index), "abc",
                {index}};
                SpinLock<WaitStrategy1> spinlock;
                if (!mpmcQueue.enqueue (obj)) {
                    spinlock.GeneralWait ();
                    if (!mpmcQueue.isRunning ()) {
                        return;
                    }
                    continue;
                }
                ALLElements[index] = obj;
                index = writeIndex.fetch_add (1, std::memory_order_acq_rel);
            }

        });
    }
    std::vector<std::thread*> readers(readerSize);
    for (int i = 0; i < readerSize; i++) {
        readers[i] = new std::thread ([&mpmcQueue, &ReadElements] () {

            while (mpmcQueue.isRunning ()) {
                //SpinLock<WaitStrategy1> spinlock;
                if (!mpmcQueue.dequeue ([&ReadElements](const auto& input) {
                    ReadElements[input.x] = input;
                })) {
                    //spinlock.GeneralWait ();
                    continue;
                }
            }
        });
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(30000));
    mpmcQueue.stop ();

    for (size_t i = 0; i < writers.size (); i++) {
        writers[i]->join ();
    }
    for (size_t i = 0; i < readers.size (); i++) {
        readers[i]->join ();
    }

    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ (ALLElements[i], ReadElements[i]);
    }
}