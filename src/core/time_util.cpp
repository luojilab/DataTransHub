#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>
#include "time_util.h"


namespace future {
    std::mutex NANO_TIME_GUARD_LOCK;


    //system sleep the time is pause
    std::int64_t TimeUtil::GetSteadyClockNanoTime(void) {
        std::lock_guard<std::mutex> lock(NANO_TIME_GUARD_LOCK);
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> nanoSteatyClockTime = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now());
        std::int64_t nanoTime =
                nanoSteatyClockTime.time_since_epoch().count();
        return nanoTime;
    }

    std::int64_t TimeUtil::GetSystemClockSecondsTime() {
        std::lock_guard<std::mutex> lock(NANO_TIME_GUARD_LOCK);
        std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp = std::chrono::time_point_cast<std::chrono::seconds>(
                std::chrono::system_clock::now());
        return tp.time_since_epoch().count();
    }

    std::int64_t TimeUtil::GetSystemClockNanoTime() {
        std::lock_guard<std::mutex> lock(NANO_TIME_GUARD_LOCK);
        std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now());
        return tp.time_since_epoch().count();
    }

    //"%Y-%m-%d %X
    //"%Y-%m-%d %H.%M.%S"
    std::string TimeUtil::GetFormatDate(const std::string &format) {
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), format.c_str());
        return ss.str();
    }
}
