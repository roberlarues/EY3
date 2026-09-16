#include "program.h"
#include "triangle.h"

using namespace ey3;

void runProgram(Engine& engine) {
	Triangle triangle;
	engine.getRenderer()->addRenderizable(&triangle);
	engine.getInputHandler()->addListener(&triangle);

	engine.run();
}
