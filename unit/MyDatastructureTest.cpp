#include <MyAny.h>
#include <MyIntegerIndexer.h>
#include <MyTuplePacker.h>
#include <Mytuple.h>
#include <TrieMap.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

using namespace datastructure;

TEST(DataStructureTest, MyTuple) {
    MyTuple xtple {"abc", 90, 999};
    EXPECT_EQ (xtple.get<0>(), "abc");
    EXPECT_EQ (xtple.get<1>(), 90);
    EXPECT_EQ (xtple.get<2>(), 999);
}

TEST(DataStructureTest, MyTuple2) {
    std::string refString = "Riri";
    const std::string name = "Rihanna Mishra";
    MyTuple xtple {refString, 90, 999};
    EXPECT_EQ (xtple.get<0>(), refString);
    EXPECT_EQ (xtple.get<1>(), 90);
    EXPECT_EQ (xtple.get<2>(), 999);
    xtple.get<0>() = name;
    EXPECT_NE (xtple.get<0>(), refString);
    EXPECT_EQ (xtple.get<0>(), name);
}
