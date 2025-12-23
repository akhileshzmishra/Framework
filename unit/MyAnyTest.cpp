#include <MyAny.h>
#include <MyIntegerIndexer.h>
#include <MyTuplePacker.h>
#include <Mytuple.h>
#include <TrieMap.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "DebugPrint.h"

using namespace datastructure;

static int ConstructorCount = 0;
static int CopyConstructorCount = 0;
static int CopyAssignmentCount = 0;
static int MoveConstructorCount = 0;
static int MoveAssignmentCount = 0;
static int DestructorCount = 0;

class AnyTestClass {
    int data_v = 90;

   public:
    AnyTestClass (int data = 90) : data_v (data) {
        ConstructorCount++;
        DebugPrint::printLine ("TestClass::TestClass () count=", ConstructorCount);
    }

    ~AnyTestClass () {
        DestructorCount++;
        DebugPrint::printLine ("TestClass::~TestClass () count=", DestructorCount);
    }

    AnyTestClass (const AnyTestClass& o) : data_v (o.data_v) {
        CopyConstructorCount++;
        DebugPrint::printLine ("TestClass::TestClass (const TestClass&) count=",
                               CopyConstructorCount);
    }

    AnyTestClass (AnyTestClass&& o) noexcept : data_v (o.data_v) {
        MoveConstructorCount++;
        DebugPrint::printLine ("TestClass::TestClass (TestClass&&) count=", MoveConstructorCount);
    }

    AnyTestClass& operator= (const AnyTestClass& o) {
        data_v = o.data_v;
        CopyAssignmentCount++;
        DebugPrint::printLine ("TestClass::operator= (const TestClass&) count=",
                               CopyAssignmentCount);
        return *this;
    }
    AnyTestClass& operator= (AnyTestClass&& o) noexcept {
        data_v = o.data_v;
        MoveAssignmentCount++;
        DebugPrint::printLine ("TestClass::operator= (TestClass&&) count=", MoveAssignmentCount);
        return *this;
    }
    friend bool operator== (const AnyTestClass& o1, const AnyTestClass& o2) {
        return o1.data_v == o2.data_v;
    }
};

void resetCounts () {
    ConstructorCount = 0;
    CopyConstructorCount = 0;
    CopyAssignmentCount = 0;
    MoveConstructorCount = 0;
    MoveAssignmentCount = 0;
    DestructorCount = 0;
}

TEST (AnyTest, testInitialization) {
    Any anyData;
    std::string stringData ("hello world");
    anyData = stringData;
    EXPECT_EQ (AnyCast<std::string> (anyData), stringData);
}

TEST (AnyTest, throwOnAnyCast) {
    Any anyData;
    std::string stringData ("hello world");
    anyData = stringData;
    EXPECT_THROW (AnyCast<int> (anyData), std::bad_cast);
}

TEST (AnyTest, reinitialize) {
    Any anyData;
    std::string stringData ("hello world");
    anyData = stringData;
    EXPECT_EQ (AnyCast<std::string> (anyData), stringData);
    anyData = 5;
    EXPECT_EQ (AnyCast<int> (anyData), 5);
}

TEST (AnyTest, reinitializeWithCustomData) {
    {
        resetCounts ();

        AnyTestClass testClass{101};
        Any anyData{testClass};
        Any secondAnyData = anyData;
        EXPECT_EQ (AnyCast<AnyTestClass> (anyData), testClass);
    }

    EXPECT_EQ ((ConstructorCount + CopyConstructorCount), DestructorCount);
}

TEST (AnyTest, reinitializeWithCustomDataOnMove) {
    {
        resetCounts ();

        AnyTestClass testClass{101};
        Any anyData{testClass};
        Any secondAnyData = std::move (anyData);
        EXPECT_EQ (AnyCast<AnyTestClass> (secondAnyData), testClass);
    }

    EXPECT_EQ ((ConstructorCount + CopyConstructorCount), DestructorCount);
    // EXPECT_EQ (MoveConstructorCount, 1);
}