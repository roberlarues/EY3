#include <android_native_app_glue.h>
#include <ey3.h>

#include "program.h"

using namespace ey3;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
	LOGI("BEGIN Android Main");

	WindowAndroid window(app);
	Engine engine(&window);
	runProgram(engine);

	LOGI("END Android Main");
}
