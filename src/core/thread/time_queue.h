#ifndef NET_DISPATCH_TIME_QUEUE_H
#define NET_DISPATCH_TIME_QUEUE_H

#include <set>
#include <functional>
#include <condition_variable>

namespace future {
    class TimeTask {

    public:
        inline TimeTask()
        : period_(0), function_(nullptr)
        , next_execution_time_(std::chrono::steady_clock::now())
        , task_id_(std::chrono::steady_clock::now()) {

        }

        inline TimeTask(unsigned long next_execution_time, unsigned long period,
                        std::function<void(void)> function) {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            period_ = period;
            function_ = function;
            auto now = std::chrono::steady_clock::now();
            task_id_ = now;
            next_execution_time_ = now + std::chrono::milliseconds(next_execution_time);
        }

        inline TimeTask(const TimeTask &other) {
            function_ = other.function_;
            period_ = other.period_;
            next_execution_time_ = other.next_execution_time_;
            task_id_ = other.task_id_;
        }

        inline TimeTask &operator=(const TimeTask &other) {
            function_ = other.function_;
            period_ = other.period_;
            next_execution_time_ = other.next_execution_time_;
            task_id_ = other.task_id_;
            return *this;
        }

        inline TimeTask(TimeTask &&other) {
            function_ = other.function_;
            other.function_ = nullptr;
            
            period_ = other.period_;
            other.period_ = 0;
            
            next_execution_time_ = other.next_execution_time_;
            
            task_id_ = other.task_id_;
        }

        inline TimeTask &operator=(TimeTask &&other) {
            std::swap(function_, other.function_);
            std::swap(period_, other.period_);
            std::swap(next_execution_time_, other.next_execution_time_);
            std::swap(task_id_, other.task_id_);
            return *this;
        }

        inline bool operator==(const TimeTask &other) const {
            return task_id_ == other.task_id_;
        }
        
        inline bool operator<(const TimeTask &other) const {
            return next_execution_time_ < other.next_execution_time_;
        }

        inline void SetFunction(std::function<void(void)> fun) {
            function_ = fun;
        }

        inline void SetPeriod(unsigned long period) {
            period_ = period;
        }
        
        inline const unsigned long GetPeriod() const {
            return period_;
        }

        inline std::function<void(void)> GetFunction() {
            return function_;
        }

        inline void SetNextExecutionTime(unsigned long next_execution_time) {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            next_execution_time_ = std::chrono::steady_clock::now() +
                                    std::chrono::milliseconds(next_execution_time);
        }

        inline std::chrono::steady_clock::time_point GetNextExecutionTime() {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            return next_execution_time_;
        }

    protected:
        std::function<void(void)> function_;                            // 回调函数
        unsigned long period_;                                          // ？？？还不知道
        std::chrono::steady_clock::time_point next_execution_time_;     // 下一个 执行时间
        std::mutex time_queue_mutex_;                                   // 互斥锁
        
    private:
        std::chrono::steady_clock::time_point task_id_;
    };

    template<typename T>
    class TimeQueue {
    public:
        typedef typename std::set<T>::iterator Iter;
        
        T PopMinimum() {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            if (time_queue_.empty()) {
                return T();
            }
            
            T ret = *(time_queue_.begin());
            time_queue_.erase(time_queue_.begin());
            return ret;
        }

        void PushBack(const T &task) {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            time_queue_.insert(task);
        }

        void Remove(const T &task) {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            Iter iter = time_queue_.begin();
            for (; iter != time_queue_.end(); ) {
                if (*iter == task) {
                    iter = time_queue_.erase(iter);
                } else {
                    iter++;
                }
            }
        }

        void Clear() {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            time_queue_.clear();
        }

        size_t Size() {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            return time_queue_.size();
        }
        
        bool Empty() {
            std::lock_guard<std::mutex> lk(time_queue_mutex_);
            return time_queue_.empty();
        }

    private:
        std::set<T> time_queue_;
        std::mutex time_queue_mutex_;
    };
}

#endif //NET_DISPATCH_TIME_QUEUE_H
