

#ifndef FIXEDLIST_H
#define FIXEDLIST_H
#include <type_traits>
#include <atomic>
#include <new>
#include <array>

namespace inplace{
template <class T>
concept ListType = std::is_default_constructible_v<T> && !std::is_const_v<T>;

template <ListType T>
class ListNode{
  public:
    using ListNodePtr = ListNode*;
    using ListNodeAtomicPtr = std::atomic<ListNode*>;
    template<class... Args>
    void construct(Args... args) {
        new (&d_data) T(std::forward<Args>(args)...);
    }

    T* data() {
        return std::launder (reinterpret_cast<T*>(d_data.get()));
    }

    ListNodeAtomicPtr& next() {
        return d_next;
    }

    ListNodeAtomicPtr& prev() {
        return d_prev;
    }

  private:
    typedef std::aligned_storage_t<sizeof(T), alignof(T)> Storage;
    Storage d_data;
    ListNodeAtomicPtr d_next{nullptr};
    ListNodeAtomicPtr d_prev{nullptr};

};

template <class M>
concept SubscribeAble = requires(M t){
    {t.subscribe()} -> std::same_as<int>;
};


template <ListType T, size_t N>
class FixedList {
    private:
    using Node = ListNode<T>;
    using NodePtr = typename Node::template ListNodeAtomicPtr<T>;
    using WriterHandle = int;
public:

    FixedList() {
        for (int i = 0; i < N; i++) {
            d_data[i].next ().store(std::addressof (d_data[(i + 1)%N]), std::memory_order_relaxed);
            d_data[i].prev ().store(std::addressof (d_data[(i + N - 1)%N]), std::memory_order_relaxed);
        }
        d_headCommitted = std::addressof(d_data[0]);
        d_tail = std::addressof(d_data[0]);
    }

    template <class ... Args>
    void write(Args...args, WriterHandle handle) {
        auto head = d_writers[handle].load(std::memory_order_relaxed);
    }

    template <SubscribeAble M>
    int subscribeWriter(M writer) {
        return d_writers.push_back(writer);
    }


private:
    NodePtr d_headCommitted{nullptr};
    NodePtr d_tail;
    NodePtr d_tailCommitted{nullptr};
    std::atomic_size_t d_size{0};
    std::array<Node, N> d_data;
    template <SubscribeAble M>
    std::vector<M> d_writers;
};


}

#endif //FIXEDLIST_H
