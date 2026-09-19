#include "maze/objects/exit.h"

#include "maze/game/depths.h"
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
	sprite.draw(getBounds(), DEPTH_EXIT);
}
