

#ifndef COMMONTESTUTILS_HPP
#define COMMONTESTUTILS_HPP



#include <gtest/gtest.h>
#include "SPSCQueue.hpp"
#include <memory>
#include <string>
#include <type_traits>

using namespace inplace;

struct DistortedStruct {
    size_t x = 0;
    size_t y = 0;
    std::shared_ptr<int> d_ptr;
    std::string vstr;
    std::vector<size_t> vvec;

    // DistortedStruct () = default;
    //
    // DistortedStruct (int a, int b, std::shared_ptr<int> d, std::string vs, std::vector<int> vec)
    //     : x (a), y (b), d_ptr (d), vstr (vs), vvec (vec) {
    // }

    bool operator== (const DistortedStruct& other) const {
        bool var = (x == other.x && y == other.y);
        if (!var)
            return false;
        if (d_ptr != other.d_ptr) {
            return false;
        }
        if (vstr != other.vstr) {
            return false;
        }

        if (vvec.size () != other.vvec.size ()) {
            return false;
        }
        for (size_t i = 0; i < vvec.size (); i++) {
            if (vvec[i] != other.vvec[i]) {
                return false;
            }
        }
        return true;
    }
};

inline std::ostream& operator<<(std::ostream& os, const DistortedStruct& obj) {
    os << obj.x << " " << obj.y << " " << obj.d_ptr << " " << obj.vstr;
    return os;
}

#endif //COMMONTESTUTILS_HPP
