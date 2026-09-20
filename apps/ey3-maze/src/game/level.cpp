#include "maze/game/level.h"

#include "maze/objects/game_object.h"

Level::Level(SpriteLibrary& sprites, int cols, int rows)
	: Scene(sprites), grid(cols, rows) {
}

void Level::onInit() {
	grid.fit(getScreenWidth(), getScreenHeight());
}

bool Level::isFree(int col, int row) const {
	if (!grid.contains(col, row)) {
		return false;
	}

	// Asking every object is fine for a board this size; a bigger game would
	// keep an index of what sits on each cell instead.
	const std::vector<GameObject*>& objects = getObjects();
	for (size_t i = 0; i < objects.size(); i++) {
		if (objects[i]->blocks(col, row)) {
			return false;
		}
	}
	return true;
}

const Grid& Level::getGrid() const {
	return grid;
}
