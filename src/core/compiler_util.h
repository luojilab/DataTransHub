#ifndef LOG_COMPILER_UTIL_H_
#define LOG_COMPILER_UTIL_H_

#if defined(_MSC_VER) && defined(MARS_USE_DLLS)
#ifdef MARS_LOG_EXPORTS
#define MARS_LOG_EXPORT __declspec(dllexport)
#else
#define MARS_LOG_EXPORT __declspec(dllimport)
#endif
#else
#define MARS_LOG_EXPORT
#endif

#endif /* LOG_COMPILER_UTIL_H_ */
