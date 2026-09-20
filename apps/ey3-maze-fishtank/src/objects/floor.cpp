#include "maze/objects/floor.h"

#include "maze/game/level.h"

Floor::Floor(Level* level, int col, int row) : GridObject(level, col, row) {
}

void Floor::onInit() {
	sprite = level->getSprites().create("sprites/floor.png");
}

void Floor::render() {
	Vec3 centre = getCellCenter();
	float size = level->getGrid().getCellSize();

	// A flat tile lying on the floor of the box.
	Mat4 model = Mat4::translation(centre.x, centre.y, centre.z) * Mat4::scaling(size, size, 1.0f);
	level->getMeshes().drawPlane(sprite, model);
}
