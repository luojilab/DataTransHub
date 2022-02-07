#include <mutex>
#include <deque>
#include <chrono>
#include <assert.h>
#include "config/platform.h"
#include "thread_manager.h"
#include "base/never_destroyed.h"

namespace future {
    static bool callbacks_paused_;

    HandlerThread *ThreadManager::handler_ = nullptr;
    std::atomic_int ThreadManager::handler_reference_count_{0};

    static std::mutex function_queue_mutex_;
    static std::mutex thread_mutex_;
    static const std::chrono::duration<double> max_run_loop_suspensionTime = std::chrono::milliseconds(
            2);

    static std::deque<std::function<void()> > &FunctionQueue() {
        static NeverDestroyed<std::deque<std::function<void()> > > function_queue;
        return function_queue;
    }

    void ThreadManager::CreateThread() {
        if (!handler_) {
            handler_ = new HandlerThread();
        }
    }

    void ThreadManager::Start() {
        if (handler_) {
            handler_->Start();
        }
    }

    void ThreadManager::DestroyThread() {
        handler_->Stop();
        handler_ = nullptr;
    }

    ThreadManager *ThreadManager::MakeThreadManager() {
        std::lock_guard<std::mutex> lock(thread_mutex_);
        if (handler_reference_count_ <= 0) {
            CreateThread();
        }
        handler_reference_count_++;

        return new ThreadManager();
    }

    void ThreadManager::DestroyThreadManager(ThreadManager *thread_manager) {
        std::lock_guard<std::mutex> lock(thread_mutex_);

        delete thread_manager;

        handler_reference_count_--;
        if (handler_reference_count_ <= 0) {
            DestroyThread();
        }
    }

    void ThreadManager::ScheduleDispatchFunctionsOnMainThread() {
        std::function<void(void)> fun = std::bind(&ThreadManager::DispatchFunctionsFromMainThread,
                                                  this);
        if (handler_) {
            handler_->PostMessage(fun);
        }
    }

    void ThreadManager::DispatchFunctionsFromMainThread() {
        assert(IsMainThread());

        if (callbacks_paused_)
            return;

        auto start_time = std::chrono::system_clock::now();

        std::function<void()> function;
        while (true) {
            {
                std::lock_guard<std::mutex> lock(function_queue_mutex_);
                if (!FunctionQueue().size()) {
                    break;
                }

                function = FunctionQueue().front();
            }

            function();
            {
                std::lock_guard<std::mutex> lock(function_queue_mutex_);
                FunctionQueue().pop_front();
            }

            function = nullptr;

            if (std::chrono::system_clock::now() - start_time > max_run_loop_suspensionTime) {
                ScheduleDispatchFunctionsOnMainThread();
                break;
            }
        }
    }

    void ThreadManager::CallOnMainThread(std::function<void()> &&function) {
        assert(function);

        bool need_to_schedule = false;

        {
            std::lock_guard<std::mutex> lock(function_queue_mutex_);
            need_to_schedule = FunctionQueue().size() == 0;
            FunctionQueue().push_back(std::move(function));
        }

        if (need_to_schedule)
            ScheduleDispatchFunctionsOnMainThread();
    }

    void ThreadManager::SetMainThreadCallbacksPaused(bool paused) {
        assert(IsMainThread());

        if (callbacks_paused_ == paused)
            return;

        callbacks_paused_ = paused;

        if (!callbacks_paused_)
            ScheduleDispatchFunctionsOnMainThread();
    }

    bool ThreadManager::IsMainThread() {
        if (!handler_) {
            return false;
        }

        assert(handler_);
        return std::this_thread::get_id() == handler_->GetThreadId();
    }
}
