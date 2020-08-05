#include <functional>
#include <thread>
#include <chrono>
#include "handler_thread.h"
#include "config/platform.h"

#if PLATFORM(ANDROID)
#include "android_util.h"
#endif

namespace future {
    
    void HandlerThread::Start() {
        std::unique_lock<std::mutex> lck(handler_thread_mutex_);
        if (!is_stop_) {
            return;
        }
        is_stop_ = false;

        std::function<void(void)> fun_run = std::bind(&HandlerThread::Run, this);
        std::thread main_thread(fun_run);
        thread_id_ = main_thread.get_id();
        main_thread.detach();
    }

    void HandlerThread::Stop() {
        std::unique_lock<std::mutex> lck(handler_thread_mutex_);
        is_stop_ = true;
        handler_thread_cond_.notify_all();
    }

    void HandlerThread::ClearTask() {
        message_queue_.Clear();
    }

    bool HandlerThread::IsQueueEmpty() {
        return message_queue_.Empty();
    }

    void HandlerThread::Run() {
#if PLATFORM(ANDROID)
        AndroidUtil::attachCurrentThread();
#endif
        while (!is_stop_) {
            TimeTask task;
            {
                std::unique_lock<std::mutex> lck(handler_thread_mutex_);
                if (message_queue_.Empty()) {
                    handler_thread_cond_.wait(lck); //如果消息队列 为空 等待锁
                }

                task = message_queue_.PopMinimum();
                if (!task.GetFunction()) {
                    continue;
                }

                auto now = std::chrono::steady_clock::now();
                if (now < task.GetNextExecutionTime()) { // 如果现在时间小于 下次执行时间 继续等待
                    handler_thread_cond_.wait_until(lck, task.GetNextExecutionTime()); //差值
                    auto wakeNow = std::chrono::steady_clock::now();
                    if (wakeNow < task.GetNextExecutionTime()) { //还是 < ，加入消息队列
                        message_queue_.PushBack(task);
                        continue;
                    }
                }

                if (task.GetPeriod() != 0) { //循环
                    task.SetNextExecutionTime(task.GetPeriod()); //设置下一次 执行时间
                    message_queue_.PushBack(task); //加入队列中
                }
            }
            
            std::function<void(void)> fun_run = task.GetFunction();
            if (fun_run) {
                fun_run();
            }
        }

#if PLATFORM(ANDROID)
        AndroidUtil::detachCurrentThread();
#endif
        delete this;
    }

    void HandlerThread::PostMessage(std::function<void(void)> msg) {
        std::unique_lock<std::mutex> lck(handler_thread_mutex_);
        TimeTask time_task(0, 0, msg);
        message_queue_.PushBack(time_task);
        handler_thread_cond_.notify_all();
    }

    void HandlerThread::PostPeriodTask(const TimeTask &task) {
        std::unique_lock<std::mutex> lck(handler_thread_mutex_);
        message_queue_.PushBack(task);
        handler_thread_cond_.notify_all();
    }

    void HandlerThread::CancelPeriodTask(const TimeTask &task) {
        message_queue_.Remove(task);
    }
}
