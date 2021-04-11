#ifndef EY3LOG_H
#define EY3LOG_H

#include <android/log.h>

#ifndef LOGNAME
#define LOGNAME "ey3"
#endif

/**
 * A shared shortcuts for logging
 */

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOGNAME, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOGNAME, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOGNAME, __VA_ARGS__))

#endif // EY3LOG_H
