#ifndef EY3WINDOW_H
#define EY3WINDOW_H

#include <cstdint>

namespace ey3 {

	class Engine;

	/**
	 * Abstracts platform window/surface/GL-context/event-loop creation so
	 * Engine and Renderer don't depend on Android or desktop windowing types
	 * directly. One implementation per platform (WindowAndroid, WindowDesktop).
	 */
	class IWindow {
		public:
			virtual ~IWindow() = default;

			// Wires the window's native lifecycle/input events back into the engine.
			virtual void setEngine(Engine* engine) = 0;

			virtual bool createSurface() = 0;
			virtual void destroySurface() = 0;
			virtual bool makeCurrent() = 0;
			virtual void swapBuffers() = 0;
			virtual void pollEvents() = 0;

			virtual int32_t getWidth() = 0;
			virtual int32_t getHeight() = 0;
	};
}

#endif // EY3WINDOW_H
