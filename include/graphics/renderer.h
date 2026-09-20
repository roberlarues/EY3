#ifndef EY3RENDERER_H
#define EY3RENDERER_H

#include <vector>

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

#include "asset_loader.h"
#include "cmd_handler.h"
#include "renderizable.h"

namespace ey3 {

	class IWindow;

	/**
	 * Render for display any Renderizable on screen
	 */
	class Renderer: public CmdHandler {
		private:
			IWindow* window;
			AssetLoader assetLoader;
			bool enabled;
			int32_t contextGeneration;
			int32_t width;
			int32_t height;

			std::vector< Renderizable* > renderizableElements;
		public:
			Renderer();
			virtual ~Renderer();
			void enable(IWindow* window);
			void disable();
			void renderFrame();
			void addRenderizable(Renderizable* renderizable);
			void removeRenderizable(Renderizable* renderizable);
			int32_t getWidth();
			int32_t getHeight();

			// The window being drawn on, for the few things that need more
			// than its pixel size (its physical size, for instance).
			IWindow* getWindow();

			/**
			 * Counts the GL contexts this renderer has had. It goes up every
			 * time the surface comes back -- which on Android happens
			 * whenever the app returns from the background, taking every
			 * texture, buffer and shader with it. Anything caching GPU
			 * resources can compare this against the last value it saw to
			 * know whether they are still alive.
			 */
			int32_t getContextGeneration();

			void handleCmd(int32_t cmd, IWindow* window);
	};
}

#endif // EY3RENDERER_H
