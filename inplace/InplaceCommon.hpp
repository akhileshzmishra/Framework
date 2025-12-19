

#ifndef INPLACECOMMON_HPP
#define INPLACECOMMON_HPP

#include <type_traits>

namespace inplace
{
    constexpr const int CACHE_LINE_SIZE = 128;

    template <class T>
    concept InplaceType = std::is_default_constructible_v<T> && !std::is_const_v<T>;
}

#endif //INPLACECOMMON_HPP
