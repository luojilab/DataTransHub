#include "period_task_manager.h"
#include "thread_manager.h"
#include "thread_base_timer.h"

namespace future {
    PeriodTaskManager *PeriodTaskManager::MakePeriodTaskManager(ThreadManager *thread_manager) {
        return new PeriodTaskManager(thread_manager);
    }
    
    void PeriodTaskManager::DestroyPeriodTaskManager(PeriodTaskManager *task_manager) {
        if (task_manager) {
            //清空该Manager下的所有延时任务
            task_manager->ClearPeriodTimeTask();
            
            //清空完成后delete操作
            delete task_manager;
        }
    }
    
    void PeriodTaskManager::AddPeriodTimeTask(Timer *timer, const TimeTask &time_task) {
        std::lock_guard<std::mutex> lk(period_task_mutex_);
        
        timers_.erase(timer); // 移除
        timers_.insert(timer);//添加
        
        if (thread_manager_) {
            HandlerThread *handler = thread_manager_->GetHandlerThread();
            if (handler) {
                handler->PostPeriodTask(time_task);
            }
        }
    }
    
    void PeriodTaskManager::RemovePeriodTimeTask(Timer *timer, const TimeTask &time_task) {
        std::lock_guard<std::mutex> lk(period_task_mutex_);
        
        timers_.erase(timer);
        
        if (thread_manager_) {
            HandlerThread *handler = thread_manager_->GetHandlerThread();
            if (handler) {
                handler->CancelPeriodTask(time_task);
            }
        }
    }
    
    void PeriodTaskManager::ClearPeriodTimeTask() {
        std::lock_guard<std::mutex> lk(period_task_mutex_);
        
        std::set<Timer *>::iterator iter = timers_.begin();
        for (; iter != timers_.end(); ++iter) {
            (*iter)->is_active_ = false;
            if (thread_manager_) {
                HandlerThread *handler = thread_manager_->GetHandlerThread();
                if (handler) {
                    handler->CancelPeriodTask(*(*iter)->time_task_);
                }
            }
        }
        timers_.clear();
    }
}
