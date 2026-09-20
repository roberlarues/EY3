#include "maze/objects/exit.h"

#include "maze/game/level.h"

// How fast the exit pulses, in frames per second.
static const float FRAME_RATE = 6.0f;

Exit::Exit(Level* level, int col, int row) : GridObject(level, col, row) {
}

void Exit::onInit() {
	sprite = level->getSprites().create("sprites/exit.png");
	sprite.setFrameRate(FRAME_RATE);
}

void Exit::update(float deltaTime) {
	sprite.update(deltaTime);
}

void Exit::render() {
	Vec3 centre = getCellCenter();
	float size = level->getGrid().getCellSize();

	Mat4 model = Mat4::translation(centre.x, centre.y, centre.z) * Mat4::scaling(size, size, 1.0f);
	level->getMeshes().drawPlane(sprite, model);
}
