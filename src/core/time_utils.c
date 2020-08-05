#include <sys/time.h>
#include <stdint.h>

#ifdef ANDROID

#include <time.h>
#include <errno.h>
#include <linux/ioctl.h>

//#if __ANDROID_API__< 21 && !defined(__LP64__)
//#include <sys/atomics.h>
//#else
#include <stdatomic.h>
//#endif

#include <fcntl.h>
#include <unistd.h>

uint64_t gettickcount() {
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec;
}

uint64_t clock_app_monotonic() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec;
}

#elif defined __APPLE__ 

#include <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) //gettimeofday will rollback, KERN_BOOTTIME up late
#include <sys/sysctl.h>
#include <assert.h>
#include <mach/kern_return.h>

uint64_t gettickcount() {
	
    // On iOS mach_absolute_time stops while the device is sleeping. Instead use
    // now - KERN_BOOTTIME to get a time difference that is not impacted by clock
    // changes. KERN_BOOTTIME will be updated by the system whenever the system
    // clock change.
    struct timeval boottime;
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    size_t size = sizeof(boottime);
    int kr = sysctl(mib, sizeof(mib)/sizeof(mib[0]), &boottime, &size, NULL, 0);
    assert(KERN_SUCCESS==kr);
    
    struct timeval now;
    gettimeofday(&now,NULL);
    
    return (uint64_t)(now.tv_sec-boottime.tv_sec) * 1000000000 + (uint64_t)(now.tv_usec-boottime.tv_usec) * 1000;
}

#else

#include <mach/mach_time.h>

uint64_t gettickcount() {
    static mach_timebase_info_data_t timebase_info = {0};
    
    // Convert to nanoseconds - if this is the first time we've run, get the timebase.
    if (timebase_info.denom == 0 )
    {
        (void) mach_timebase_info(&timebase_info);
    }
    
    // Convert the mach time to milliseconds
    uint64_t mach_time = mach_absolute_time();
    uint64_t millis = (mach_time * timebase_info.numer) / (timebase_info.denom);
    return millis * 1000000;
}
#endif

uint64_t clock_app_monotonic() {
    return gettickcount();
}

#elif defined Q_OS_BLACKBERRY

#include <time.h>

uint64_t gettickcount() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
    return 0;
}

#elif defined __linux__

#include <time.h>

uint64_t gettickcount() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
    return 0;
}

uint64_t clock_app_monotonic() {
    return gettickcount();
}

#elif defined _WIN32
//#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

uint64_t gettickcount() {
    return GetTickCount();
}

uint64_t clock_app_monotonic() {
    return gettickcount();
}
#elif WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || UWP 

#include "unistd.h"
#include <stdint.h>

uint64_t gettickcount() {
    return GetTickCount64();
}
uint64_t clock_app_monotonic() {
    return gettickcount();
}

#else
#error "not support"
#endif

int64_t gettickspan(uint64_t _old_tick) {
    uint64_t cur_tick = gettickcount();
    if (_old_tick > cur_tick) return 0;

    return cur_tick - _old_tick;
}

uint64_t timeMs() {
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}
