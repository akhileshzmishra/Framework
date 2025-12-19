

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

template <size_t Number>
consteval bool isPowerOf2 () {
    return (Number & (Number - 1)) == 0;
}

static_assert (isPowerOf2<1> ());
static_assert (isPowerOf2<2> ());
static_assert (!isPowerOf2<3> ());
static_assert (isPowerOf2<4> ());
static_assert (!isPowerOf2<5> ());
static_assert (!isPowerOf2<6> ());
static_assert (!isPowerOf2<7> ());
static_assert (isPowerOf2<8> ());

template <int Size = 16>
union SizedPointerStorage {
    static_assert (Size >= 16, "Size must be greater than 16");
    static_assert (isPowerOf2<Size> (), "Size must be power of 2");
    char data[Size];
    void* voidPointer;
    const void* constVoidPointer;
    void (*functionPointer) ();
    void (Undefined_class::*mMemberFunctionPointer) ();
};

template <int Size = 16>
union AnyData {

    alignas (std::max_align_t) char bytes[sizeof (SizedPointerStorage<Size>)];

    void* asVoidPtr(){
        return static_cast<void*>(&bytes[0]);
    }

    [[nodiscard]] const void* asVoidPtr () const { return static_cast<const void*> (&bytes[0]); }

    template <class T>
    T* asPtr () {
        // With std::launder is to tell compiler that to forget any object existed here.
        return std::launder (static_cast<T*> (asVoidPtr ()));
    }

    template <class T>
    const T* asPtr () const {
        return std::launder (static_cast<const T*> (asVoidPtr ()));
    }
};
}  // namespace datastructure
#endif //ANYSTORAGE_H
