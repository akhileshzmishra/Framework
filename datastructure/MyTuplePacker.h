

#ifndef MYTUPLEPACKER_H
#define MYTUPLEPACKER_H

#include <iostream>
#include <ostream>
#include <tuple>
#include <concepts>
#include <array>
#include <algorithm>
#include <type_traits>
#include <condition_variable>
#include <mutex>

namespace datastructure {

template<class...Args>
class Packer{
    using Tuple = std::tuple<Args...>;
    Tuple member;
public:
    explicit Packer(Args...args): member(args...) {}

    template<int position>
    auto& get(){
        return std::get<position>(member);
    }

    template<class F> requires std::is_invocable_v<F, Args...>
    void unpackFor(F&& functor){

        [this]<std::size_t... I>
            (Tuple&& tuple, F&& f, std::index_sequence<I...>)
        {
            (f(std::get<I>(tuple)...));
        }(
            std::forward<Tuple>(member),
            std::forward<F>(functor),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
        );
    }

    template<class...F> requires (std::is_invocable_v<F, Args> && ... )
    void unpackEach(F&&...functor){

        [&functor...]<std::size_t... I>
            (Tuple&& tuple, std::index_sequence<I...>)
        {
            (functor(std::get<I>(tuple)),...);
        }(
            std::forward<Tuple>(member),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}
        );
    }
};
}
#endif //MYTUPLEPACKER_H
