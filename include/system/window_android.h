#ifndef EY3WINDOWANDROID_H
#define EY3WINDOWANDROID_H

#include <android_native_app_glue.h>
#include <EGL/egl.h>

#include "window.h"

namespace ey3 {

	/**
	 * IWindow backed by an android_app's ANativeWindow, via EGL.
	 */
	class WindowAndroid: public IWindow {
		private:
			android_app* app;
			Engine* engine = nullptr;
			EGLDisplay display = EGL_NO_DISPLAY;
			EGLSurface surface = EGL_NO_SURFACE;
			EGLContext context = EGL_NO_CONTEXT;
			int32_t width = 0;
			int32_t height = 0;

		public:
			WindowAndroid(android_app* app);

			void setEngine(Engine* engine) override;
			bool createSurface() override;
			void destroySurface() override;
			bool makeCurrent() override;
			void swapBuffers() override;
			void pollEvents() override;
			int32_t getWidth() override;
			int32_t getHeight() override;
	};
}

#endif // EY3WINDOWANDROID_H
