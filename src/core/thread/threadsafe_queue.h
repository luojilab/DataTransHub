#ifndef NET_DISPATCHER_THREADSAFE_QUEUE_H
#define NET_DISPATCHER_THREADSAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace future{
    template<typename T>
    class ThreadsafeQueue {
    public:
        void PushBack(T new_data) {
            std::lock_guard <std::mutex> lk(threadsafe_queue_mutex_);            // 1.全局加锁
            threadsafe_queue_data_.push(std::move(new_data));           // 2.push时独占锁
            threadsafe_queue_cond_.notify_one();
        }

        void WaitAndPop(T &val) {
            std::unique_lock <std::mutex> ulk(threadsafe_queue_mutex_);                    // 3.全局加锁
            threadsafe_queue_cond_.wait(ulk, [this]() { return !threadsafe_queue_data_.empty(); });  // 4.front 和 pop_front时独占锁
            val = std::move(threadsafe_queue_data_.front());
            threadsafe_queue_data_.pop();
        }

        std::shared_ptr <T> WaitAndPop() {
            std::unique_lock <std::mutex> ulk(threadsafe_queue_mutex_);
            threadsafe_queue_cond_.wait(ulk, [this]() { return !threadsafe_queue_data_.empty(); });
            std::shared_ptr <T> val(std::make_shared<T>(std::move(threadsafe_queue_data_.front())));
            threadsafe_queue_data_.pop();
            return val;
        }

        bool TryPop(T &val) {
            std::lock_guard <std::mutex> lk(threadsafe_queue_mutex_);
            if (threadsafe_queue_data_.empty()) {
                return false;
            }
            val = std::move(threadsafe_queue_data_.front());
            threadsafe_queue_data_.pop();
            return true;
        }

        std::shared_ptr <T> TryPop() {
            std::shared_ptr <T> val;
            std::lock_guard <std::mutex> lk(threadsafe_queue_mutex_);
            if (threadsafe_queue_data_.empty()) {
                return val;
            }
            val = std::make_shared<T>(std::move(threadsafe_queue_data_.front()));
            threadsafe_queue_data_.pop();
            return val;
        }

        bool Empty() {
            std::lock_guard <std::mutex> lk(threadsafe_queue_mutex_);
            return threadsafe_queue_data_.empty();
        }

        void Clear() {
            std::lock_guard <std::mutex> lk(threadsafe_queue_mutex_);
            std::queue<T> empty;
            swap(empty, threadsafe_queue_data_);
        }

    private:
        std::queue<T> threadsafe_queue_data_;
        std::mutex threadsafe_queue_mutex_;
        std::condition_variable threadsafe_queue_cond_;
    };
}
#endif
