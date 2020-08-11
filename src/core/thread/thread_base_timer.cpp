#include "thread_base_timer.h"
#include "time_queue.h"
#include "period_task_manager.h"

namespace future{
    
    Timer::Timer(std::function<void()>&& function,
                 unsigned long next_execution_time,
                 unsigned long period_time )
    : next_execution_time_(next_execution_time) {
        time_task_ = std::make_shared<TimeTask>(next_execution_time, period_time, function);
    }
    
    Timer::~Timer() {
        if (is_active_) {
            Stop();
        }
    }
    
    void Timer::SetFunction(std::function<void()>&& function) {
        time_task_->SetFunction(function);
    }
    
    void Timer::SetNextExecutionTime(unsigned long next_execution_time) {
        next_execution_time_ = next_execution_time;
    }
    
    unsigned long Timer::GetNextExecutionTime() {
        return next_execution_time_;
    }
    
    void Timer::SetPeriodTime(unsigned long period_time) {
        time_task_->SetPeriod(period_time);
    }
    
    void Timer::Stop() {
        if (!is_active_) {
            return;
        }
        
        if (period_task_manager_) {
            period_task_manager_->RemovePeriodTimeTask(this, *time_task_.get());
        }
        
        is_active_ = false;
    }
    
    void Timer::StartWithPeriodTaskManager(PeriodTaskManager *period_task_manager,
                                           bool delayWithPeriod) {
        if (is_active_) {
            return;
        }
        
        if (!period_task_manager) {
            return;
        }
        
        if (!time_task_) {
            return;
        }
        
        
        if (delayWithPeriod) {
            next_execution_time_ = time_task_->GetPeriod();
        }
        
        time_task_->SetNextExecutionTime(next_execution_time_);
        
        is_active_ = true;
        period_task_manager_ = period_task_manager;
        period_task_manager_->AddPeriodTimeTask(this, *time_task_.get());
    }
    
    void Timer::ReStartWithPeriodTaskManager(PeriodTaskManager *period_task_manager,
                                             unsigned long next_execution_time) {
        if (is_active_) {
            Stop();
        }
        
        if (!period_task_manager) {
            return;
        }
        
        if (!time_task_) {
            return;
        }
        
        next_execution_time_ = next_execution_time;
        
        StartWithPeriodTaskManager(period_task_manager, false);
    }
}
