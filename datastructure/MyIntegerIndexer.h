//
// Created by Akhilesh Mishra on 03/04/2025.
//

#ifndef MYINTEGERINDEXER_H
#define MYINTEGERINDEXER_H

namespace datastructure {


template <int...I>
struct IntegerIndex{};

template <int CurrentSize, int FixedSize, int...I>
constexpr auto makeReverseIndexHelper(){
    if constexpr(CurrentSize == FixedSize){
        return IntegerIndex<I...>();
    }
    else {
        return makeReverseIndexHelper<CurrentSize + 1, FixedSize, CurrentSize, I...>();
    }
}

template <int N>
constexpr auto makeReverseIndex(){
    return makeReverseIndexHelper<0, N>();
}

template <int Size, int...I>
constexpr auto makeForwardIndexHelper() {
    if constexpr (Size == 0) {
        return IntegerIndex<I...>();
    }
    else {
        return makeForwardIndexHelper<Size - 1, Size + 1, I...>();
    }
}

template <int Size>
constexpr auto makeForwardIndex() {
    return makeForwardIndexHelper<Size>();
}

}
#endif //MYINTEGERINDEXER_H
