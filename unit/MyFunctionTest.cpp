

#include <MyFunction.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace datastructure;

class TestData {
    std::vector<int> vec;

   public:
    TestData () { DebugPrint::printLine ("TestData::TestData()"); }
    TestData (const TestData&) { DebugPrint::printLine ("TestData::TestData(const TestData&)"); }
    TestData (TestData&&) { DebugPrint::printLine ("TestData::TestData(TestData&&)"); }

    ~TestData () { DebugPrint::printLine ("TestData::~TestData()"); }
    void operator() () { DebugPrint::printLine ("TestData::operator()()"); }
};

int g(const int & x) {
    std::cout << x << std::endl;
    return x;
}

TEST(MyFunctionTest, testMyFunction)
{
    MyFunction mf (g);
    std::function afn (g);
    std::cout << mf (5) << std::endl;
    int a = 10;
    mf (a);
}

TEST (MyFunctionTest, testMyFunction2) {
    MyFunction mf ([] (const int& x) { return x + 1; });
    std::function afn ([] (const int& x) { return x + 1; });
    MyFunction mf2 = std::move (mf);
}

TEST (MyFunctionTest, testMyFunction3) {
    std::vector<std::string> vec = {"abcd", "abcd", "abcd"};
    MyFunction mf ([vec] () {
        for (const auto& v : vec) std::cout << v << std::endl;
    });
    mf ();
}

TEST (MyFunctionTest, testMyFunction4) {
    TestData td;
    MyFunction mf (td);
    DebugPrint::printLine ("---------------------");
    mf ();
    DebugPrint::printLine ("---------------------");
}