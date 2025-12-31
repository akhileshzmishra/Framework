#ifndef INPLACESTACK_HPP
#define INPLACESTACK_HPP
#include <array>
namespace inplace {

template <typename T>
concept StackableLike = std::is_nothrow_default_constructible_v<T>;

template <StackableLike T, int Size>
class InPlaceStack {
   public:
    InPlaceStack ();

    ~InPlaceStack () = default;

    template <class UT>
        requires std::is_same_v<T, UT>
    void Push (UT &&);

    void Pop ();

    T &Top ();

    [[nodiscard]] bool Empty () const;

    [[nodiscard]] bool Full () const;

   private:
    using Cell = std::aligned_storage_t<sizeof (T), alignof (T)>;
    std::array<Cell, Size> stack;
    int d_top;
    int d_size;
};

template <StackableLike T, int Size>
InPlaceStack<T, Size>::InPlaceStack () : d_top (0), d_size (0) {}

template <StackableLike T, int Size>
template <class UT>
    requires std::is_same_v<T, UT>
void InPlaceStack<T, Size>::Push (UT &&ut) {
    new (stack[d_top]) UT (std::forward<UT> (ut));
    d_top++;
}

template <StackableLike T, int Size>
void InPlaceStack<T, Size>::Pop () {
    stack[d_top--].~T ();
    d_top--;
}

template <StackableLike T, int Size>
auto InPlaceStack<T, Size>::Top () -> T & {
    return std::launder (stack[d_top]);
}

template <StackableLike T, int Size>
bool InPlaceStack<T, Size>::Empty () const {
    return d_top == 0;
}

template <StackableLike T, int Size>
bool InPlaceStack<T, Size>::Full () const {
    return d_top == Size;
}
}  // namespace inplace
#endif //INPLACESTACK_HPP
