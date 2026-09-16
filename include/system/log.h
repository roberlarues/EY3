#ifndef EY3LOG_H
#define EY3LOG_H

#ifndef LOGNAME
#define LOGNAME "ey3"
#endif

/**
 * A shared shortcuts for logging
 */

#ifdef __ANDROID__

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOGNAME, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOGNAME, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOGNAME, __VA_ARGS__))

#else

#include <cstdio>

#define LOGI(fmt, ...) ((void)std::fprintf(stdout, "I/" LOGNAME ": " fmt "\n", ##__VA_ARGS__))
#define LOGW(fmt, ...) ((void)std::fprintf(stderr, "W/" LOGNAME ": " fmt "\n", ##__VA_ARGS__))
#define LOGE(fmt, ...) ((void)std::fprintf(stderr, "E/" LOGNAME ": " fmt "\n", ##__VA_ARGS__))

#endif // __ANDROID__

#endif // EY3LOG_H
