#include <ey3.h>

#include "program.h"

using namespace ey3;

/**
 * Desktop entry point, mirroring android_main's structure so app logic
 * (Triangle, in program.cpp) is shared unmodified between platforms.
 */
int main() {
	LOGI("BEGIN Desktop Main");

	WindowDesktop window("EY3 Triangle", 800, 600);
	Engine engine(&window);
	engine.onCmd(APP_CMD_INIT_WINDOW);
	runProgram(engine);

	LOGI("END Desktop Main");
	return 0;
}
