//
// Created by Akhilesh Mishra on 21/12/2024.
//

#include <gtest/gtest.h>
#include "SPSCQueue.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <CommonTestUtils.hpp>
#include <Signal.hpp>
using namespace inplace;


TEST (SPSCQueueTest, enqueue) {
    inplace::SPSCQueue<DistortedStruct, 35> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    for (size_t i = 0; i < 35; i++) {
        vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
        q.enqueue (vecWriter.back ());
    }

    EXPECT_EQ (q.size (), 35);
    EXPECT_EQ (q.full (), true);

    while (!q.empty ()) {
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


TEST (SPSCQueueTest, waitTill) {
    inplace::SPSCQueue<DistortedStruct, 35> q;
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

TEST (SPSCQueueTest, DISABLED_threadedWrite) {
    constexpr const size_t QSize = 35;
    SPSCQueue<DistortedStruct, QSize> q;
    std::vector<DistortedStruct> vecWriter;
    std::vector<DistortedStruct> vecReader;
    test::Signal s1, s2, s3;

    std::thread writer ([&vecWriter, &q, &s1, &s2, QSize] {
        for (size_t i = 0; i < QSize; i++) {
            vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
            bool expect = q.enqueue (vecWriter.back ());
            EXPECT_EQ (expect, true);
        }

        EXPECT_EQ (q.size (), QSize);
        EXPECT_EQ (q.full (), true);

        for (size_t i = 0; i < 1; i++) {
            bool expect = q.enqueue (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
            EXPECT_EQ (expect, false);
            s1.signal ();
        }
        s1.reset();
        s2.waitFor();

        for (size_t i = 0; i < QSize; i++) {
            vecWriter.push_back (DistortedStruct{ i, i, std::make_shared<int> (i), "ak", { i } });
            bool expect = q.enqueue (vecWriter.back ());
            EXPECT_EQ (expect, true);
        }

        s1.signal ();

    });

    std::thread reader ([&vecReader, &q, &s1, &s2, QSize] {
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
            auto elem = q.dequeue ([&vecReader](auto& x) {
                vecReader.push_back (x);
            });
            EXPECT_EQ (elem, false);
            EXPECT_EQ (q.full (), false);
            EXPECT_EQ (q.empty (), true);
        }
        s2.signal();
        s1.waitFor ();

        for (size_t i = 0; i < QSize; i++) {
            auto elem = q.dequeue ([&vecReader](auto& x) {
                vecReader.push_back (x);
            });
            EXPECT_EQ (elem, true);
        }

    });
    writer.join ();
    reader.join ();

    EXPECT_EQ (vecReader.size (), vecWriter.size ());

    for (size_t i = 0; i < vecReader.size (); i++) {
        EXPECT_EQ (vecReader.at (i), vecWriter.at (i)) << vecReader.at (i) << ", " << vecWriter.at (i);
        EXPECT_EQ (vecReader.at (i).d_ptr, vecWriter.at (i).d_ptr);
    }

}