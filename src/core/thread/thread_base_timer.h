#ifndef NET_DISPATCH_THREAD_BASE_TIMER_H
#define NET_DISPATCH_THREAD_BASE_TIMER_H

#include <queue>
#include <functional>
#include <condition_variable>
#include <map>
#include "time_queue.h"

namespace future{
    
    class PeriodTaskManager;
    
    class Timer {

    public:
        Timer(std::function<void()>&& function = nullptr,
              unsigned long period_time = 0,
              unsigned long next_execution_time = 0);
        ~Timer();
        
        void SetFunction(std::function<void()>&& function);
        
        void SetPeriodTime(unsigned long period_time);
        
        void SetNextExecutionTime(unsigned long next_execution_time);
        
        unsigned long GetNextExecutionTime();
        
        void StartWithPeriodTaskManager(PeriodTaskManager *period_task_manager, bool delayWithPeriod);
        
        void ReStartWithPeriodTaskManager(PeriodTaskManager *period_task_manage,
                                          unsigned long next_execution_time);
        
        bool IsActive() { return is_active_; }
        
        void Stop();
        
    private:
        friend class PeriodTaskManager;
        
        std::shared_ptr<TimeTask> time_task_ { nullptr };
        PeriodTaskManager *period_task_manager_ { nullptr };
        std::atomic_bool is_active_ { false };
        
        unsigned long next_execution_time_;
    };
}

#endif //NET_DISPATCH_THREAD_BASE_TIMER_H
