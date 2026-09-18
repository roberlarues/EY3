#include <android_native_app_glue.h>
#include <ey3.h>

#include "program.h"

using namespace ey3;

void android_main(struct android_app* app) {
	LOGI("BEGIN android_main");

	WindowAndroid window(app);
	Engine engine(&window);

	runProgram(engine);

	LOGI("END android_main");
}
