#ifndef EY3APPCMD_H
#define EY3APPCMD_H

/**
 * Logical lifecycle command codes shared by every platform. On Android these
 * are literally android_native_app_glue's APP_CMD_* values (re-exported as-is
 * so Camera and other Android-only CmdListeners keep working unmodified). On
 * desktop they're EY3-owned values with no external meaning.
 */
#ifdef __ANDROID__

#include <android_native_app_glue.h>

#else

enum {
	APP_CMD_INPUT_CHANGED = 0,
	APP_CMD_INIT_WINDOW,
	APP_CMD_TERM_WINDOW,
	APP_CMD_WINDOW_RESIZED,
	APP_CMD_WINDOW_REDRAW_NEEDED,
	APP_CMD_CONTENT_RECT_CHANGED,
	APP_CMD_GAINED_FOCUS,
	APP_CMD_LOST_FOCUS,
	APP_CMD_CONFIG_CHANGED,
	APP_CMD_LOW_MEMORY,
	APP_CMD_START,
	APP_CMD_RESUME,
	APP_CMD_SAVE_STATE,
	APP_CMD_PAUSE,
	APP_CMD_STOP,
	APP_CMD_DESTROY,
};

#endif // __ANDROID__

#endif // EY3APPCMD_H
