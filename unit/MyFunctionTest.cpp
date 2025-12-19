

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
    struct DataVector {
        int eleents[45] = {
            53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
        };
    };
    DataVector vec;
    MyFunction mf ([vec] () { std::cout << vec.eleents[0] << std::endl; });
    mf ();
}

TEST (MyFunctionTest, testMyFunction4) {
    TestData td;
    MyFunction mf (td);
    DebugPrint::printLine ("---------------------");
    mf ();
    DebugPrint::printLine ("---------------------");
}

TEST (MyFunctionTest, testClassStaticFunction) {
    class AClass {
       public:
        static void method () { std::cout << "method" << std::endl; }
    };

    MyFunction mf (AClass::method);
    DebugPrint::printLine ("---------------------");
    mf ();
}
