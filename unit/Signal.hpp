

#ifndef SIGNAL_HPP
#define SIGNAL_HPP
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>


namespace test {
    class Signal {
        std::mutex mutex;
        std::condition_variable condition;
        std::atomic<bool> stop = false;

    public:
        Signal() {
            stop = false;
        }
        void reset() {
            stop = false;
        }
        void signal() {
            stop = true;
            condition.notify_all();
        }

        void wait() {
            std::unique_lock<std::mutex> lock(mutex);
            while (!stop) {
                condition.wait(lock, [this] {
                    return stop.load(std::memory_order_acquire);
                });
            }
        }

        bool waitFor(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
            std::unique_lock<std::mutex> lock(mutex);
            if (condition.wait_for(lock, timeout) != std::cv_status::no_timeout) {
                return stop;
            }
            return stop;
        }
    };
}
#endif //SIGNAL_HPP
