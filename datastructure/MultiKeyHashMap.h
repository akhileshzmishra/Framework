//
// Created by Akhilesh Mishra on 27/01/2025.
//

#ifndef MULTIKEYHASHMAP_H
#define MULTIKEYHASHMAP_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <concepts>
#include <iostream>
#include <ostream>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace datastructure {


template <class... Tn>
    requires (std::is_nothrow_convertible_v<Tn, int> && ...)
struct TupleHash {

    int64_t operator() (std::tuple<Tn...> tuple) const {
        constexpr auto tSize = sizeof...(Tn);
        constexpr int bitSize = 60 / (tSize);
        constexpr int mask = (1 << bitSize) - 1;
        int64_t hash = 0;
        int maskItr = mask;
        for (size_t i = 0; i < tSize; ++i) {
            auto tnum = std::get<0>(tuple);
            hash |= (tnum & maskItr);
            maskItr <<= bitSize;
        }

        return hash;
    }
};

template <class K, class... Args>
requires (std::is_nothrow_convertible_v<K, int> && ... && std::is_nothrow_convertible_v<Args, int>)
class Key {
    using InternalTuple = std::tuple<K, Args...>;
    InternalTuple d_key;

   public:
    explicit Key (K&& k, Args&&... args)
        : d_key (std::make_tuple (std::forward<K> (k), std::forward<Args> (args)...)) {}

    bool operator== (const Key& k) const { return d_key == k.d_key; }

    template <class M, class... MArgs>
    requires (std::is_nothrow_convertible_v<K, M> && ... &&
        std::is_nothrow_convertible_v<Args, MArgs>)
    Key (const Key<M, MArgs...>& k) : d_key (k.d_key) {}

    const InternalTuple& get () const { return d_key; }
};

template <class K, class... Args>
std::ostream& operator<< (std::ostream& ostr, const Key<K, Args...>& k) {
    std::apply ([&] (const auto... x) { (std::cout << ... << x); }, k.get ());
    return ostr;
}

template <class T, class K1, class... KN>
    requires (std::is_nothrow_convertible_v<K1, int> && ... &&
              std::is_nothrow_convertible_v<KN, int>)
class HashMapPair {

    template <class K, class... Args>
        requires (std::is_nothrow_convertible_v<Args, int> && ...)
    class SpecialHash {
       public:
        int64_t operator() (const Key<K, Args...>& k1) const {
            TupleHash<K, Args...> tHash;
            return tHash (k1.get ());
        }
    };

    using InternalMap = std::unordered_map<Key<K1, KN...>, T, SpecialHash<K1, KN...>>;
    InternalMap umap;

   public:
    using InternalItr = typename InternalMap::iterator;
    void emplace (T&& t, K1&& k1, KN&&... kn) {
        umap.emplace (std::make_pair (
            Key<K1, KN...> (std::forward<K1> (k1), std::forward<KN> (kn)...), t));
    }

    void emplace (const T& t, const K1& k1, const KN&... kn) {
        umap.emplace (std::make_pair (Key<K1, KN...>
            (std::forward<K1> (k1), std::forward<KN> (kn)...), t));
    }

    T& operator() (K1&& k1, KN&&... kn) {
        return umap[Key< K1, KN...> (std::forward< K1> (k1),
                                                std::forward< KN> (kn)...)];
    }

    const T& operator() (const K1& k1, const KN&... kn) const {
        return umap[Key< K1,  KN...>
            (std::forward< K1> (k1),  std::forward< KN> (kn)...)];
    }

    bool find (K1&& k1, KN&&... kn) {
        Key<K1, KN...> key (std::forward<K1> (k1), std::forward<KN> (kn)...);
        return (umap.find (key) != umap.end ());
    }

    bool find (const K1& k1, const KN&... kn) const {
        Key<K1, KN...> key
            (std::forward<K1> (k1), std::forward<KN> (kn)...);
        return (umap.find (key) != umap.end ());
    }

    InternalItr begin () { return umap.begin (); }

    InternalItr end () { return umap.end (); }
};
}  // namespace datastructure
#endif  // MULTIKEYHASHMAP_H
