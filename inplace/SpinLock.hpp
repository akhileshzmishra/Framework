

#ifndef SPINLOCK_HPPdd
#define SPINLOCK_HPPdd
#include <array>
#include <atomic>
#include <concepts>
#include <thread>
#include <tuple>
#include <InplaceCommon.hpp>
#include <memory>
#include <chrono>
#include <type_traits>

namespace inplace
{
    template <class T>
    concept SpinLockWaitStrategy = requires(T t)
    {
        { t.wait() } -> std::same_as<bool>;
    };

    template <class T>
    concept Integral = std::is_integral<T>::value;


    template <size_t MaxSpin, size_t maxDelay, size_t adder, size_t yieldTimed, size_t multiplier,
    size_t maxTimeInMicroSec>
    struct WaitStrategy
    {
    private:
        static constexpr size_t MAX_SPINNER_WITHOUT_SLEEP = MaxSpin;
        static constexpr size_t DELAY_CONSUMER_MICROSECONDS = maxDelay;
        static constexpr size_t ADDER = adder;
        static constexpr size_t YIELD_AFTER_MICROSECONDS = yieldTimed;
        static constexpr size_t MULTIPLIER = multiplier;
        size_t d_spinner = 0;
        size_t d_beginSleep = DELAY_CONSUMER_MICROSECONDS;
        static constexpr std::chrono::microseconds MAX_DELAY = std::chrono::microseconds
        (maxTimeInMicroSec);
        std::chrono::time_point<std::chrono::system_clock> d_begin = std::chrono::system_clock::now();

    public:
        WaitStrategy() = default;

        [[nodiscard]] bool wait()
        {
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            auto elapsed_time = std::chrono::duration<int64_t, std::micro>(d_begin - now).count();
            if (elapsed_time > MAX_DELAY.count())
            {
                return false;
            }
            constexpr const auto yieldTime = YIELD_AFTER_MICROSECONDS;
            if (d_beginSleep >= yieldTime)
            {
                d_beginSleep = DELAY_CONSUMER_MICROSECONDS;
                std::this_thread::yield();
            }
            if (d_spinner < MAX_SPINNER_WITHOUT_SLEEP)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(d_beginSleep));
                d_beginSleep = nextDelay(d_beginSleep);
            }
            d_spinner++;
            return true;
        }

        void reset() {
            d_spinner = 0;
            d_begin = std::chrono::system_clock::now();
        }

    private:
        static size_t nextDelay(size_t delay)
        {
            return delay * MULTIPLIER + ADDER;
        }
    };

    using WaitStrategy1 = WaitStrategy<50, 200, 100, 1000, 1, 1000000>;
    using WaitStrategy2 = WaitStrategy<5, 1000, 1000, 10000, 1, 1000000>;

    using WaitStrategyYield = WaitStrategy<2, 10, 10, 100, 1, 1000>;
    using WaitStrategyExponential = WaitStrategy<5, 1000, 0, 10000, 2, 1000000>;

    template <SpinLockWaitStrategy Strategy = WaitStrategy1>
    class SpinLock
    {
    public:
        template <Integral T>
        static bool YieldAfter(std::atomic<T>& valueToObserve, T&&
                               expectedValue, T&& replacementValue)
        {
            Strategy strategy;
            T expected = expectedValue;
            while (!valueToObserve.compare_exchange_strong(expected,
                                                           std::forward<T>(replacementValue),
                                                           std::memory_order_acquire))
            {
                if (!strategy.wait())
                {
                    return false;
                }
                expected = expectedValue;
            }
            return true;
        }

        static void GeneralWait() {
            Strategy strategy;
            if (!strategy.wait()) {
                std::this_thread::yield();
            }
        }
    };
}

#endif //SPINLOCK_HPP
