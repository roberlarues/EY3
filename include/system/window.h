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

			// Physical size of the drawing surface, in millimetres, or 0
			// when the platform cannot tell. Anything that has to reason in
			// real world units needs this -- a head-coupled perspective, for
			// instance, has to know how big its window on the world is.
			virtual float getPhysicalWidthMm() { return 0.0f; }
			virtual float getPhysicalHeightMm() { return 0.0f; }
	};
}

#endif // EY3WINDOW_H
