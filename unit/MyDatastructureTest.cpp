#include <gtest/gtest.h>
#include <Mytuple.h>
#include <MyTuplePacker.h>
#include <MyIntegerIndexer.h>
#include <iostream>
#include <MyAny.h>
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

TEST(DataStructureTest, AnyTest){
    Any anyD(std::string("akhilesh"));
    Any anyB;
    anyB = 69;
    EXPECT_EQ(AnyCast<std::string>(anyD), "akhilesh");
    EXPECT_EQ(AnyCast<int>(anyB), 69);
    //std::cout << AnyCast<int>(anyB) << std::endl;
}


TEST(DataStructureTest, AnyTestUnorderemap){
    std::unordered_map<std::string, Any> anyMap;
    anyMap["Akhilesh"] = std::string("akhilesh");
    anyMap["Rihanna"] = 42;
    std::cout << AnyCast<int>(anyMap["Rihanna"] )<< std::endl;
    std::cout << AnyCast<std::string>(anyMap["Akhilesh"]) << std::endl;
    //EXPECT_EQ (AnyCast<int> (mymap["Mishra"]), 20);
}