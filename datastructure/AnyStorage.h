//
// Created by Akhilesh Mishra on 04/05/2025.
//

#ifndef A_NYSTORAGE_H
#define A_NYSTORAGE_H

#include <memory>
#include <type_traits>
#include <iostream>
#include <ostream>
#include <tuple>
#include <bitset>
#include <concepts>
#include <array>
#include <cmath>
#include <algorithm>

namespace datastructure {
class Undefined_class;

union PtrDataStorage
{
    void*       object;
    const void* constObject;
    void (*functionPointer)();
    void (Undefined_class::*mMemberFunctionPointer)();
};

union AnyData{
    PtrDataStorage d_data;
    char bytes[sizeof(PtrDataStorage)];

    void* asVoidPtr(){
        return static_cast<void*>(&bytes[0]);
    }

    [[nodiscard]] const void* asVoidPtr() const{
        return static_cast<const void*>(&bytes[0]);
    }

    template <class T>
    T* asPtr(){
        return static_cast<T*>(asVoidPtr());
    }

    template <class T>
    const T* asPtr() const{
        return static_cast<const T*>(asVoidPtr());
    }
};
}
#endif //ANYSTORAGE_H
