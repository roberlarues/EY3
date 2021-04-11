#ifndef EY3ENGINE_H
#define EY3ENGINE_H

#include <android_native_app_glue.h>

#include "input_handler.h"
#include "cmd_handler.h"
#include "renderer.h"

namespace ey3 {

	/**
	 * Engine base for an easy use of the lib.
	 */
	class Engine {
		private:
			struct android_app* app;
			Renderer renderer;
			InputHandler inputHandler;
			CmdHandler cmdHandler;
			bool foreground = false;
			bool terminated = false;

		public:
			Engine(struct android_app* app);
			~Engine();
			void onCmd(int32_t cmd);
			void onInput(AInputEvent* event);
			void pollEvents();

			Renderer* getRenderer();
			InputHandler* getInputHandler();
			CmdHandler* getCmdHandler();
			bool isInForeground();
			bool hasTerminated();
	};

}

#endif // EY3ENGINE_H
