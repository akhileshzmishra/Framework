#include <ZipView.h>
#include <gtest/gtest.h>

#include <CommonTestUtils.hpp>
#include <Signal.hpp>
#include <memory>
#include <string>
#include <type_traits>
using namespace inplace;

TEST (ZipView, ZipView) {
    std::vector<int> data1 = {1, 2, 3, 4, 5};
    std::vector<int> data2 = {1, 2, 3, 4};
    std::vector<int> ans;
    for (const auto [a, b] : ZipView{data1, data2}) {
        EXPECT_EQ (a, b);
        ans.push_back (a);
    }

    EXPECT_EQ (ans.size (), 4);
}

TEST (ZipView, DifferentViews) {
    std::vector<int> data1 = {1, 2, 3, 4, 5};
    std::map<int, int> data2 = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
    std::vector<int> ans;

    for (const auto [a, b] : ZipView{data1, data2}) {
        EXPECT_EQ (a, b.first);
        ans.push_back (b.second);
    }

    EXPECT_EQ (ans.size (), 4);
}