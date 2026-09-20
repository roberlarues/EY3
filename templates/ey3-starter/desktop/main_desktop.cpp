#include <ey3.h>

#include "program.h"

using namespace ey3;

int main() {
	LOGI("BEGIN app");

	WindowDesktop window("EY3 App", MOBILE_WIDTH_PORTRAIT, MOBILE_HEIGHT_PORTRAIT);
	Engine engine(&window);
	engine.onCmd(APP_CMD_INIT_WINDOW);

	runProgram(engine);

	LOGI("END app");
	return 0;
}
