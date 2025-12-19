
#include <gtest/gtest.h>
#include <MultiKeyHashMap.h>
#include <iostream>

using namespace datastructure;

TEST(MultiKeyHashMapTest, DISABLED_create) {
    HashMapPair<int, int, int> hmp;
    hmp.emplace(1, 2, 3);
    hmp.emplace(1, 5, 3);
    TupleHash<int, int> thash;
    //std::cout <<hmp.find(2, 3) << std::endl;
    std::cout << "thash=" << thash(Key< int, int>(56, 3).get()) << std::endl;
    for (const auto& i: hmp){
        std::cout<< i.first << "->" << i.second << std::endl;
    }

    std::cout << "thash2=" << thash(Key<int, int>(2, 3).get()) << std::endl;
}