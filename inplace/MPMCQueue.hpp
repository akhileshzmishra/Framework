//
// Created by Akhilesh Mishra on 23/12/2024.
//

#ifndef MPMCQUEUE_HPP
#define MPMCQUEUE_HPP

#include <array>
#include <atomic>
#include <concepts>
#include <thread>
#include <tuple>
#include <InplaceCommon.hpp>
#include <memory>
#include <stdalign.h>
#include <type_traits>
#include <functional>
#include <SpinLock.hpp>

#include "InplaceOstream.hpp"

namespace inplace {
template <class T>
concept MPMCLockFreeType = std::is_default_constructible_v<T> && !std::is_const_v<T>;

struct MPMCNodeTraits {
    static constexpr int NULL_INDEX = -1;
};

template <Integral T>
struct alignas(inplace::CACHE_LINE_SIZE) MPMCIndexTraits {
    std::atomic<T> index = 0;
    int identity = 0;
};

template <MPMCLockFreeType T>
class MPMCNode {
    public:
    static constexpr const int64_t NEUTRAL = 1;
    static constexpr const int64_t WRITING = 2;
    static constexpr const int64_t READING = 3;
    static constexpr const int64_t WRITTEN = 4;

    private:
    typedef std::aligned_storage_t<sizeof (T), alignof (T)> StorageType;
    mutable StorageType d_data;
    std::atomic_int64_t d_state = MPMCNode::NEUTRAL;

    public:
    template <class... Args>
    T* create (Args... args) {
        return ::new (&d_data) T (std::forward<Args> (args)...);
    }

    const T& asData () const {
        return *std::launder (reinterpret_cast<T*> (&d_data));
    }

    T& asData () {
        return *std::launder (reinterpret_cast<T*> (&d_data));
    }

    void resetFlag () {
        d_state.store (NEUTRAL, std::memory_order_release);
    }

    void setWrittenFlag () {
        d_state.store (WRITTEN, std::memory_order_release);
    }

    std::atomic_int64_t& state () {
        return d_state;
    }

    std::string statusAsString () const {
        static const std::string names[] = {
            "NEUTRAL",
            "WRITING",
            "READING",
            "WRITTEN",
            "INVALID"
        };
        return names[d_state.load (std::memory_order::acquire)];
    }
};

template <MPMCLockFreeType T>
std::ostream& operator << (std::ostream& os, const MPMCNode<T>& node) {
    os << "[" << node.asData () << "] " << node.statusAsString ();
    return os;
}

template <MPMCLockFreeType T, int64_t Capacity, bool debug = true, SpinLockWaitStrategy Strategy =
                              WaitStrategy1>
class MPMCQueue : public MPMCNodeTraits {
    public:
    MPMCQueue () {
        d_writerCommitted.index.store (0, std::memory_order::relaxed);
    }

    ~MPMCQueue () = default;

    MPMCQueue (MPMCQueue const&) = delete;

    MPMCQueue& operator= (MPMCQueue const&) = delete;

    MPMCQueue (MPMCQueue&&) = delete;

    MPMCQueue& operator= (MPMCQueue&&) = delete;

    void stop () {
        d_stop.store (true, std::memory_order_release);
    }

    bool isRunning () const {
        return !d_stop.load (std::memory_order_acquire);
    }

    void print () {
        std::cout << std::this_thread::get_id () << std::endl;
        for (int i = 0; i < Capacity; i++) {
            std::cout << d_queue[i] << std::endl;
        }
    }

    template <class... Args>
    bool enqueue (Args&&... args) {
        if (full (std::memory_order_acquire)) {
            InplaceOstream<debug>::print ("full");
            return false;
        }

        int64_t index = getNextWriteIndex ();
        if (index == NULL_INDEX) {
            return false;
        }
        auto& node = d_queue[index % Capacity];
        node.create (std::forward<Args&&> (args)...);
        node.setWrittenFlag ();

        advanceWriteCommitted (index);

        increaseSize ();
        return true;
    }

    template <std::invocable<const T&> Callable>
    bool dequeue (Callable&& callable) {
        if (empty (std::memory_order_acquire)) {
            InplaceOstream<debug>::print ("empty");
            return false;
        }

        if (d_reader.index.load (std::memory_order_relaxed) >= d_writerCommitted.index.load (
                std::memory_order_relaxed)
        ) {
            if (d_reader.index.load (std::memory_order_acquire) >= d_writerCommitted.index.load (
                    std::memory_order_acquire)
            ) {
                InplaceOstream<debug>::print ("empty2");
                return false;
            }
        }

        int64_t index = getNextReadIndex ();

        if (index == NULL_INDEX) {
            return false;
        }

        auto& node = d_queue[index % Capacity];
        std::invoke (std::forward<Callable> (callable), node.asData ());
        node.asData ().~T ();

        node.resetFlag ();

        decreaseSize ();
        return true;
    }

    size_t size (std::memory_order m = std::memory_order_acquire) const {
        return d_size.load (m);
    }

    bool empty (std::memory_order m = std::memory_order_acquire) const {
        return d_size.load (m) == 0;
    }

    bool full (std::memory_order m = std::memory_order_acquire) const {
        return d_size.load (m) == Capacity;
    }

    private:
    [[nodiscard]] int64_t getNextWriteIndex () {
        int64_t index = d_writer.index.fetch_add (1, std::memory_order_acquire);
        Strategy strategy;
        auto& node = d_queue[index % Capacity];
        auto& state = node.state ();
        int64_t expectedState = MPMCNode::NEUTRAL;
        while (!state.compare_exchange_strong (expectedState, MPMCNode::WRITING,
                                               std::memory_order_acquire)) {
            if (!isRunning ()) {
                index = NULL_INDEX;
                break;
            }
            expectedState = MPMCNode::NEUTRAL;
            if (!strategy.wait ()) {
                strategy.reset ();
            }
        }
        return index;
    }

    [[nodiscard]] int64_t getNextReadIndex () {
        int64_t index = d_reader.index.fetch_add (1, std::memory_order_acquire);
        Strategy strategy;
        auto& node = d_queue[index % Capacity];
        auto& state = node.state ();
        int64_t expectedState = MPMCNode::WRITTEN;
        while (!state.compare_exchange_strong (expectedState, MPMCNode::READING,
                                               std::memory_order_acquire)) {
            if (!isRunning ()) {
                index = NULL_INDEX;
                break;
            }
            expectedState = MPMCNode::WRITTEN;
            if (!strategy.wait ()) {
                strategy.reset ();
            }
        }
        return index;
    }

    void increaseSize () {
        d_size.fetch_add (1, std::memory_order_release);
    }

    void decreaseSize () {
        d_size.fetch_sub (1, std::memory_order_release);
    }

    void advanceWriteCommitted (int64_t currIndex) {
        int64_t lastPotentialIndex = currIndex;
        while (!d_writerCommitted.index.compare_exchange_strong (
            lastPotentialIndex, currIndex + 1, std::memory_order_acquire)) {
            lastPotentialIndex = currIndex;
        }
    }

    void advanceReadCommitted (int64_t currIndex) {
        int64_t lastPotentialIndex = currIndex;
        while (!d_readerCommitted.index.compare_exchange_strong (
            lastPotentialIndex, currIndex + 1, std::memory_order_acquire)) {
            lastPotentialIndex = currIndex;
        }
    }

    private:
    using MPMCNode = MPMCNode<T>;
    std::atomic<size_t> d_size = 0;
    std::atomic_bool d_stop = false;
    std::array<MPMCNode, Capacity> d_queue;
    MPMCIndexTraits<int64_t> d_writer;
    MPMCIndexTraits<int64_t> d_writerCommitted;
    MPMCIndexTraits<int64_t> d_reader;
    MPMCIndexTraits<int64_t> d_readerCommitted;
};
}

#endif //MPMCQUEUE_HPP