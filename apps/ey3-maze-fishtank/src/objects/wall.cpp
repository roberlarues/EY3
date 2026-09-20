#include "maze/objects/wall.h"

#include "maze/game/level.h"

Wall::Wall(Level* level, int col, int row) : GridObject(level, col, row) {
}

void Wall::onInit() {
	sprite = level->getSprites().create("sprites/wall.png");
}

void Wall::render() {
	Vec3 centre = getCellCenter();
	float size = level->getGrid().getCellSize();

	// A cube standing on the floor, rising towards the player.
	Mat4 model = Mat4::translation(centre.x, centre.y, centre.z + size / 2.0f)
		* Mat4::scaling(size, size, size);
	level->getMeshes().drawCube(sprite, model);
}

bool Wall::blocks(int col, int row) const {
	return col == this->col && row == this->row;
}
