#ifndef EY3RENDERIZABLE_H
#define EY3RENDERIZABLE_H

#include <android_native_app_glue.h>

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

namespace ey3 {

	/**
	 * Interface that must implement renderizable elements.
	 * They should be added to the Renderer for display.
	 */
	class Renderizable {
		public:
			virtual void init(android_app* app) = 0;
			virtual void render() = 0;
	};
}

#endif // EY3RENDERIZABLE_H
