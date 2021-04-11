#include <string>
#include <android_native_app_glue.h>
#include <ey3.h>

#include "triangle.h"

using namespace ey3;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
	LOGI("BEGIN Android Main");
	Engine engine(app);

	Triangle triangle;
	engine.getRenderer()->addRenderizable(&triangle);
	engine.getInputHandler()->addListener(&triangle);
	
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			engine.getRenderer()->renderFrame();
		}
	}

	LOGI("END Android Main");
}

