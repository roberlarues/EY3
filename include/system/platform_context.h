#ifndef EY3PLATFORMCONTEXT_H
#define EY3PLATFORMCONTEXT_H

struct android_app;

namespace ey3 {

	/**
	 * There is exactly one android_app per process (guaranteed by
	 * native_app_glue), so it's exposed here instead of threading it through
	 * every leaf class (AssetLoader, ...) that occasionally needs it.
	 * Android-only; not used or compiled on desktop.
	 */
	namespace PlatformContext {
		void setAndroidApp(android_app* app);
		android_app* getAndroidApp();
	}
}

#endif // EY3PLATFORMCONTEXT_H
