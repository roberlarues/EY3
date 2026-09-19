#include "maze/objects/floor.h"

#include "maze/game/depths.h"
#include "maze/game/level.h"

Floor::Floor(Level* level, int col, int row) : GridObject(level, col, row) {
}

void Floor::onInit() {
	sprite = level->getSprites().create("sprites/floor.png");
}

void Floor::render() {
	sprite.draw(getBounds(), DEPTH_FLOOR);
}
