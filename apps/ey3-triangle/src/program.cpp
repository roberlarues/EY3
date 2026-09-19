#include "program.h"
#include "triangle.h"

using namespace ey3;

void runProgram(Engine& engine) {
	Triangle triangle;
	engine.getRenderer()->addRenderizable(&triangle);
	engine.getInputHandler()->addListener(&triangle);

	// The loop belongs to the app, not to the engine: pump the platform's
	// events, then draw, until the window goes away.
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			engine.getRenderer()->renderFrame();
		}
	}
}
