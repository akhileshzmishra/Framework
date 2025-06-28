//
// Created by Akhilesh Mishra on 08/02/2025.
//

#include <MyFunction.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace datastructure;

template<class T>
T f(T&& i){
    std::cout << i << std::endl;
    return i;
}

int g(const int & x) {
    std::cout << x << std::endl;
    return x;
}

TEST(MyFunctionTest, testMyFunction)
{
    MyFunction< int(const int&)> mf(g);
    std::cout << mf(5) << std::endl;
    int a = 10;
    mf(a);
}