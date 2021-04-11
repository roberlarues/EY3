#ifndef EY3RENDERER_H
#define EY3RENDERER_H

#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <vector>

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

#include "cmd_handler.h"
#include "renderizable.h"

namespace ey3 {

	/**
	 * Render for display any Renderizable on screen
	 */
	class Renderer: public CmdHandler {
		private:
			android_app* app;
			bool enabled;
			EGLDisplay display;
			EGLSurface surface;
			EGLContext context;
			int32_t width;
			int32_t height;

			std::vector< Renderizable* > renderizableElements;
		public:
			Renderer();
			virtual ~Renderer();
			void enable(android_app* app);
			void disable();
			void renderFrame();
			void addRenderizable(Renderizable* renderizable);
			void removeRenderizable(Renderizable* renderizable);
			int32_t getWidth();
			int32_t getHeight();

			void handleCmd(int32_t cmd, android_app* app);
	};
}

#endif // EY3RENDERER_H
