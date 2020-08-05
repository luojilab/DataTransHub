#ifndef NET_DISPATCH_HANDLER_THREAD_H
#define NET_DISPATCH_HANDLER_THREAD_H

#include <thread>
#include <atomic>
#include "threadsafe_queue.h"
#include "time_queue.h"

namespace future {
    class HandlerThread {
    public:
        void Start();

        void Stop();

        void PostMessage(std::function<void(void)> msg);

        void PostPeriodTask(const TimeTask &task);

        void CancelPeriodTask(const TimeTask &task);

        void ClearTask();

        bool IsQueueEmpty();
    
        std::thread::id GetThreadId() { return thread_id_; }

    private:
        ~HandlerThread() { }
    
        void Run();

        void ExecuteTask();

        void ExecutePeriodTask();

    private:
        std::thread::id thread_id_;
        std::atomic_bool is_stop_ { true };
        TimeQueue<TimeTask> message_queue_;
        std::mutex handler_thread_mutex_;
        std::condition_variable handler_thread_cond_;
    };
}

#endif //NET_DISPATCH_HANDLER_THREAD_H
