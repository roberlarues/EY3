#include <ey3.h>

#include "program.h"

using namespace ey3;

/**
 * Desktop entry point, mirroring android_main's structure so app logic
 * (Camera/CameraView/OrbTest wiring, in program.cpp) is shared unmodified
 * between platforms. Camera captures from the system's default webcam.
 */
int main() {
	LOGI("BEGIN Desktop Main");

	// screenOrientation="landscape" in AndroidManifest.xml.
	WindowDesktop window("EY3 ORB Demo", MOBILE_WIDTH_LANDSCAPE, MOBILE_HEIGHT_LANDSCAPE);
	Engine engine(&window);
	engine.onCmd(APP_CMD_INIT_WINDOW);
	runProgram(engine);

	LOGI("END Desktop Main");
	return 0;
}
