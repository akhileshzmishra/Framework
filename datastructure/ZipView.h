#ifndef ZIPVIEW_H
#define ZIPVIEW_H
#include <iterator>
#include <ranges>
#include <tuple>

template <typename T>
concept ZipViewConcept = std::ranges::common_range<T>;

template <typename T>
concept ZipViewIteratorConcept = std::forward_iterator<T>;

template <ZipViewIteratorConcept... Iterators>
class ZipViewIterator final {
    using IteratorType = std::tuple<Iterators...>;
    using DataType = std::tuple<std::add_lvalue_reference_t<typename Iterators::value_type>...>;
    IteratorType d_iterators;

   public:
    explicit ZipViewIterator (Iterators... iterators) : d_iterators (iterators...) {}

    auto operator++ () -> IteratorType& {
        []<size_t... I> (IteratorType& iterator, std::index_sequence<I...>) {
            ((std::get<I> (iterator) = std::next (std::get<I> (iterator))), ...);
        }(d_iterators, std::make_index_sequence<sizeof...(Iterators)>{});
        return d_iterators;
    }
    auto operator++ (int) -> IteratorType& {
        []<size_t... I> (IteratorType& iterator, std::index_sequence<I...>) {
            ((std::get<I> (iterator) = std::next (std::get<I> (iterator))), ...);
        }(d_iterators, std::make_index_sequence<sizeof...(Iterators)>{});
        return d_iterators;
    }

    bool operator== (const ZipViewIterator& other) const {
        return
            []<size_t... I> (const std::tuple<Iterators...>& iterator1,
                             const std::tuple<Iterators...>& iterator2, std::index_sequence<I...>) {
                return ((std::get<I> (iterator1) == std::get<I> (iterator2)) || ...);
            }(d_iterators, other.d_iterators, std::make_index_sequence<sizeof...(Iterators)>{});
    }

    DataType operator* () {
        return []<size_t... I> (IteratorType& iterator, std::index_sequence<I...>) {
            return DataType (*std::get<I> (iterator)...);
        }(d_iterators, std::make_index_sequence<sizeof...(Iterators)>{});
    }
};

template <ZipViewConcept... Views>
class ZipView {
    std::tuple<Views&...> d_views;
    using Iterator = ZipViewIterator<typename Views::iterator...>;

   public:
    explicit ZipView (Views&... d_views) : d_views (d_views...) {}

    Iterator begin () {
        return []<size_t... I> (std::tuple<Views&...>& view, std::index_sequence<I...>) {
            return ZipViewIterator (std::begin (std::get<I> (view))...);
        }(d_views, std::make_index_sequence<sizeof...(Views)>{});
    }

    Iterator end () {
        return []<size_t... I> (std::tuple<Views&...>& view, std::index_sequence<I...>) {
            return ZipViewIterator (std::end (std::get<I> (view))...);
        }(d_views, std::make_index_sequence<sizeof...(Views)>{});
    }
};

#endif  // ZIPVIEW_H
