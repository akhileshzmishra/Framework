#ifndef INPLACESTACK_HPP
#define INPLACESTACK_HPP
#include <array>
namespace inplace {
    /**
     * @brief 
     * @tparam T
     * @tparam Size 
     */
    template<class T, int Size>
    class InPlaceStack {
    public:
        InPlaceStack();

        ~InPlaceStack() = default;

        void Push(const T &);

        void Pop();

        T &Top();

        [[nodiscard]] bool Empty() const;

        [[nodiscard]] bool Full() const;

    private:
        std::array<T, Size> stack;
        int d_top;
        int d_size;
    };

    template<class T, int Size>
    InPlaceStack<T, Size>::InPlaceStack(): d_top(0), d_size(0) {
    }

    template<class T, int Size>
    void InPlaceStack<T, Size>::Push(const T &data) {
        stack[d_top] = data;
        d_top++;
    }

    template<class T, int Size>
    void InPlaceStack<T, Size>::Pop() {
        d_top--;
    }

    template<class T, int Size>
    T &InPlaceStack<T, Size>::Top() {
        return stack[d_top];
    }

    template<class T, int Size>
    bool InPlaceStack<T, Size>::Empty() const {
        return d_top == 0;
    }

    template<class T, int Size>
    bool InPlaceStack<T, Size>::Full() const {
        return d_top == Size;
    }
}
#endif //INPLACESTACK_HPP
