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

			void handleCmd(int32_t cmd, IWindow* window);
	};
}

#endif // EY3RENDERER_H
