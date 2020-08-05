#ifndef LOG_H
#define LOG_H

#include "platform.h"

#if PLATFORM(ANDROID)

#include <android/log.h>

#endif

#include <cstring>
#include <errno.h>

namespace future {
#if PLATFORM(ANDROID)
// enable logging
#define ENABLE_LOG

#ifdef ENABLE_LOG

#define APPNAME "DataTransHub"

#define Error(format, ...)                                                                     \
    __android_log_print(ANDROID_LOG_ERROR, APPNAME, format, ##__VA_ARGS__)
#define Warning(format, ...)                                                                   \
    __android_log_print(ANDROID_LOG_WARN, APPNAME, format, ##__VA_ARGS__)
#define Info(format, ...) __android_log_print(ANDROID_LOG_INFO, APPNAME, format, ##__VA_ARGS__)

#ifndef NDEBUG
#define Debug(format, ...)                                                                     \
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, format, ##__VA_ARGS__)
#else
#define Debug(format, ...)                                                                     \
      {}
#endif

#else

#endif

#else
#define Error(format, ...)                                                                     \
    {}
#define Warning(format, ...)                                                                   \
    {}
#define Info(format, ...)                                                                      \
    {}
#define Debug(format, ...)                                                                     \
    {}
#endif

    inline void print_hex(unsigned char *buf, uint64_t len) {
        //Print results:
        int i;
        for (i = 0; i < len; i++) {
            Debug("%02X", buf[i]);
            if (15 == i % 16)
                Debug("\n");
        }
        Debug("\n");
    }

}
#endif //LOG_H
