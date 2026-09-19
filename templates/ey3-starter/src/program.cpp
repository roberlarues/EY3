#include "program.h"

using namespace ey3;

/**
 * Empty scene: nothing registered yet, so the engine just clears the screen
 * every frame. Add your own ey3::Renderizable (see ey3-triangle in the main
 * EY3 repo for a minimal working example) and register it below.
 */
void runProgram(Engine& engine) {
	// MyShape shape;
	// engine.getRenderer()->addRenderizable(&shape);
	// engine.getInputHandler()->addListener(&shape);

	// The loop belongs to the app, not to the engine: pump the platform's
	// events, then draw, until the window goes away.
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			engine.getRenderer()->renderFrame();
		}
	}
}
