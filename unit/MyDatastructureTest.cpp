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

TEST (DataStructureTest, TrieMap_set_valid) {
    TrieMap<char, std::string> trieMap;
    trieMap.set ("Akhilesh", "akhilesh");
    EXPECT_TRUE (trieMap.get ("Akhilesh").has_value ());
    EXPECT_EQ (trieMap.get ("Akhilesh").value ().get (), std::string ("akhilesh"));
}

TEST (DataStructureTest, TrieMap_test_invalid) {
    TrieMap<char, std::string> trieMap;
    trieMap.set ("Akhilesh", "akhilesh");
    EXPECT_FALSE (trieMap.get ("Akhilesh1").has_value ());
}