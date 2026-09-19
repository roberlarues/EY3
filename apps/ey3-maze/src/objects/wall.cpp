#include "maze/objects/wall.h"

#include "maze/game/depths.h"
#include "maze/game/level.h"

Wall::Wall(Level* level, int col, int row) : GridObject(level, col, row) {
}

void Wall::onInit() {
	sprite = level->getSprites().create("sprites/wall.png");
}

void Wall::render() {
	sprite.draw(getBounds(), DEPTH_WALL);
}

bool Wall::blocks(int col, int row) const {
	return col == this->col && row == this->row;
}
