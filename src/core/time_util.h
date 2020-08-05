#ifndef NanoTime_hpp
#define NanoTime_hpp

#include <cstdint>

#ifdef __cplusplus /* 如果采用了C++，如下代码使用C编译器 */
extern "C" { /* 如果没有采用C++，顺序预编译 */
#endif

namespace future {
    class TimeUtil {
    public:
        static std::int64_t GetSteadyClockNanoTime(void);

        static std::int64_t GetSystemClockSecondsTime();

        static std::int64_t GetSystemClockNanoTime();

        static std::string GetFormatDate(const std::string &format);
    };

}
#ifdef __cplusplus /* 结束使用C编译器 */
}
#endif

#endif /* NanoTime_hpp */

