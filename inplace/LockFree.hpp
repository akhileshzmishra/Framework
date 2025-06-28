// //
// // Created by Akhilesh Mishra on 15/12/2024.
// //
//
// #ifndef INPLACELOCKFREE_HPP
// #define INPLACELOCKFREE_HPP
//
// #include <array>
// #include <atomic>
// #include <concepts>
// #include <thread>
//
// namespace inplace {
//     template<class T>
//     concept LockFreeType = std::is_default_constructible_v<T> && std::is_const_v<T>;
//
//     struct LFNodeTraits {
//         static constexpr int NULL_INDEX = -1;
//     };
//
//     struct RWTraits {
//         std::atomic_long d_commitFinished = 0;
//         std::atomic_long d_commitPending = 0;
//         std::atomic_bool d_isUsed = false;
//         int identity = 0;
//     };
//
//     template <LockFreeType T>
//     class LockFreeNode {
//         T d_data;
//         std::atomic_bool d_inUse = false;
//     public:
//         T& data() { return d_data; }
//         void setInUse() { d_inUse.store(true, std::memory_order_relaxed); }
//         void setCommitFinished() { d_inUse.store(false, std::memory_order_relaxed); }
//         bool isInUse() const { return d_inUse.load(std::memory_order_acquire); }
//         std::atomic_bool& atomic() { return d_inUse;}
//     };
//
//     template<LockFreeType T, int Capacity>
//     class LockFreeQueue : public LFNodeTraits {
//     public:
//         LockFreeQueue() = default;
//
//         ~LockFreeQueue() = default;
//
//         LockFreeQueue(LockFreeQueue const &) = delete;
//
//         LockFreeQueue &operator=(LockFreeQueue const &) = delete;
//
//         LockFreeQueue(LockFreeQueue &&) = delete;
//
//         LockFreeQueue &operator=(LockFreeQueue &&) = delete;
//
//         template<std::invocable F, class... Args>
//         void enqueue(T const &element,F&& invocable, Args&&... args) {
//             auto index = getNextWriteIndex();
//             while (d_queue[index].atomic().compare_exchange_strong(false, true, std::memory_order_acquire)) {
//             }
//             std::invoke(std::forward<F>(invocable), element, args);
//         }
//
//         T dequeue(int readerId);
//     private:
//         long getNextWriteIndex() {
//             if (d_size.load() == Capacity) {
//                 return NULL_INDEX;
//             }
//             auto currentIndex = d_currentWriteIndex.load(std::memory_order_acquire);
//             while (!d_currentWriteIndex.compare_exchange_strong(currentIndex, currentIndex + 1, std::memory_order_relaxed)) {
//                 currentIndex = d_currentWriteIndex.load(std::memory_order_acquire);
//             }
//
//             std::atomic_thread_fence(std::memory_order_release);
//             return currentIndex;
//         }
//
//     private:
//         using Node = LockFreeNode<T>;
//         std::atomic_long d_size = 0;
//         std::array<Node, Capacity> d_queue;
//         std::atomic_long d_currentWriteIndex = 0;
//         std::atomic_long d_currentWriteCommitted = 0;
//         std::atomic_long d_currentReadIndex = 0;
//         std::atomic_long d_currentReadCommitted = 0;
//     };
// }
//
// #endif //INPLACELOCKFREE_HPP
