//
// Created by Akhilesh Mishra on 03/04/2025.
//

#ifndef MYTUPLE_H
#define MYTUPLE_H

#include <iostream>

namespace datastructure {

template <int index, class First, class...Rest>
struct MyTupleHelper: MyTupleHelper<index + 1, Rest...>{
    First value;
    using Parent = MyTupleHelper<index + 1, Rest...>;

    explicit constexpr MyTupleHelper(First&& f, Rest&&...r):
    Parent(std::forward<Rest>(r)...),
    value(std::forward<First>(f))
    {

    }

    template <int I>
    constexpr auto& get(){
        if constexpr(I == index){
            return value;
        }
        else{
            return Parent::template get<I>();
        }
    }
};
template <int index, class T>
struct MyTupleHelper<index, T>{
    T value;

    explicit constexpr MyTupleHelper(T&& val): value(std::forward<T>(val)) {}
    template <int I>
    constexpr auto& get(){
        if constexpr(I == index){
            return value;
        }
        else{
            static_assert(I <= index, "index out of bounds");
        }
    }
};

template <class...Args>
struct MyTuple : MyTupleHelper<0, Args...>{
    using Parent = MyTupleHelper<0, Args...>;

    explicit MyTuple(Args...args): Parent(std::forward<Args>(args)...) {}

    template <int I>
    constexpr auto& get(){
        return Parent::template get<I>();
    }
};

}
#endif //MYTUPLE_H
