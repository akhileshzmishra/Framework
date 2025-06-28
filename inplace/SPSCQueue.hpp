//
// Created by Akhilesh Mishra on 20/12/2024.
//

#ifndef SPSCQUEUE_HPP
#define SPSCQUEUE_HPP

#include <array>
#include <atomic>
#include <concepts>
#include <thread>
#include <tuple>
#include <InplaceCommon.hpp>
#include <memory>
#include <stdalign.h>
#include <type_traits>
#include <SpinLock.hpp>
#include <ranges>

namespace inplace
{
    template <class T>
    concept LockFreeType = std::is_default_constructible_v<T> && !std::is_const_v<T>;

    struct LFNodeTraits
    {
        static constexpr int NULL_INDEX = -1;
    };

    template <Integral T>
    struct alignas(inplace::CACHE_LINE_SIZE) RWTraits
    {
        std::atomic<T> index = 0;
        int identity = 0;
    };

    template <LockFreeType T>
    class LockFreeNode
    {
        typedef std::aligned_storage_t<sizeof(T), alignof(T)> StorageType;
        StorageType d_data;
        std::atomic_bool d_inUse = false;

    public:
        template <class... Args>
        T*create(Args... args)
        {
            return ::new(&d_data) T(std::forward<Args>(args)...);
        }

        T& asData()
        {
            return *std::launder(reinterpret_cast<T*>(&d_data));
        }

        void setInUse() { d_inUse.store(true, std::memory_order_relaxed); }
        void setCommitFinished() { d_inUse.store(false, std::memory_order_relaxed); }
        bool isInUse() const { return d_inUse.load(std::memory_order_acquire); }
        std::atomic_bool& atomic() { return d_inUse; }
    };

    template <LockFreeType T, int64_t Capacity>
    class SPSCQueue : public LFNodeTraits
    {
    public:
        SPSCQueue() = default;

        ~SPSCQueue() = default;

        SPSCQueue(SPSCQueue const&) = delete;

        SPSCQueue& operator=(SPSCQueue const&) = delete;

        SPSCQueue(SPSCQueue&&) = delete;

        SPSCQueue& operator=(SPSCQueue&&) = delete;

        template <class... Args>
        bool enqueue(Args&&... args)
        {
            if (full(std::memory_order_acquire))
            {
                return false;
            }
            int64_t index = d_writer.index.load(std::memory_order_acquire);
            d_queue[index % Capacity].create(std::forward<Args&&>(args)...);
            d_writer.index.store(index + 1, std::memory_order_relaxed);
            d_size.fetch_add(1, std::memory_order_release);
            std::atomic_thread_fence(std::memory_order_release);
            return true;
        }

        template <std::invocable<const T&> Callable>
        bool dequeue(Callable&& callable)
        {
            if (empty(std::memory_order_acquire))
            {
                return false;
            }
            int64_t index = d_reader.index.load(std::memory_order_relaxed);
            callable(d_queue[index % Capacity].asData());
            d_queue[index % Capacity].asData().~T();
            d_reader.index.store(index + 1, std::memory_order_relaxed);
            d_size.fetch_sub(1, std::memory_order_release);
            return true;
        }

        size_t size(std::memory_order m = std::memory_order_acquire) const { return d_size.load(m); }
        bool empty(std::memory_order m = std::memory_order_acquire) const { return d_size.load(m) == 0; }
        bool full(std::memory_order m = std::memory_order_acquire) const { return d_size.load(m) == Capacity; }

    private:
        using Node = LockFreeNode<T>;
        std::atomic<size_t> d_size = 0;
        std::array<Node, Capacity> d_queue;
        RWTraits<int64_t> d_writer;;
        RWTraits<int64_t> d_reader;;
    };
}

#endif //SPSCQUEUE_HPP
