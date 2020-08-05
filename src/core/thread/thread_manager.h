#ifndef NET_DISPATCH_THREAD_MANAGER_H
#define NET_DISPATCH_THREAD_MANAGER_H

#include <functional>
#include "handler_thread.h"

namespace future {
    class ThreadManager {
    public:
        static ThreadManager *MakeThreadManager();
        
        static void DestroyThreadManager(ThreadManager *thread_manager);
        
        bool IsMainThread();

        void Start();

        void CallOnMainThread(std::function<void()>&&);

        void Cancel(std::function<void()>&&);
        
        HandlerThread *GetHandlerThread() { return handler_; }
        
    private:
        ThreadManager() { }
        
        ~ThreadManager() { }

        void SetMainThreadCallbacksPaused(bool paused);

        void ScheduleDispatchFunctionsOnMainThread();
        
        void DispatchFunctionsFromMainThread();
        
        static void CreateThread();
        static void DestroyThread();
        
    private:
        static HandlerThread *handler_;
        static std::atomic_int handler_reference_count_;
    };
}

#endif //NET_DISPATCH_THREAD_MANAGER_H
