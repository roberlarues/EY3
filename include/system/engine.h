#ifndef EY3ENGINE_H
#define EY3ENGINE_H

#include <chrono>

#include "input_event.h"
#include "input_handler.h"
#include "cmd_handler.h"
#include "renderer.h"
#include "window.h"

namespace ey3 {

	/**
	 * Engine base for an easy use of the lib.
	 */
	class Engine {
		private:
			IWindow* window;
			Renderer renderer;
			InputHandler inputHandler;
			CmdHandler cmdHandler;
			bool foreground = false;
			bool terminated = false;
			std::chrono::steady_clock::time_point lastFrameTime;
			float deltaTime = 0.0f;

		public:
			Engine(IWindow* window);
			~Engine();
			void onCmd(int32_t cmd);
			void onInput(const InputEvent& event);
			void pollEvents();

			// Standard loop: pumps events and renders while in foreground,
			// until the engine terminates. Apps with extra per-frame work
			// (e.g. polling a camera) can call pollEvents/renderFrame
			// directly instead and write their own loop.
			void run();

			IWindow* getWindow();
			Renderer* getRenderer();
			InputHandler* getInputHandler();
			CmdHandler* getCmdHandler();
			bool isInForeground();
			bool hasTerminated();
			float getDeltaTime();
	};

}

#endif // EY3ENGINE_H
