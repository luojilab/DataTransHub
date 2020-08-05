#ifndef NET_DISPATCH_PERIOD_TASK_MANAGER_H
#define NET_DISPATCH_PERIOD_TASK_MANAGER_H

#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <memory>

#include "threadsafe_queue.h"
#include "time_queue.h"
#include "handler_thread.h"

namespace future {
    class ThreadManager;
    class Timer;
    
    class PeriodTaskManager {
    public:
        static PeriodTaskManager *MakePeriodTaskManager(ThreadManager *thread_manager);
        
        static void DestroyPeriodTaskManager(PeriodTaskManager *);
        
    private:
        PeriodTaskManager(ThreadManager *thread_manager)
            : thread_manager_(thread_manager) { }
        
        ~PeriodTaskManager() { }
        
        void AddPeriodTimeTask(Timer *timer, const TimeTask &time_task);
        
        void RemovePeriodTimeTask(Timer *timer, const TimeTask &time_task);
        
        void ClearPeriodTimeTask();
        
    private:
        friend class Timer;
        
        ThreadManager *thread_manager_ { nullptr };
        std::set<Timer *> timers_;
        std::mutex period_task_mutex_;
    };
}

#endif //NET_DISPATCH_PERIOD_TASK_MANAGER_H
