#include "maze/objects/grid_object.h"

#include "maze/game/level.h"

GridObject::GridObject(Level* level, int col, int row)
	: GameObject(level), level(level), col(col), row(row) {
}

int GridObject::getCol() const {
	return col;
}

int GridObject::getRow() const {
	return row;
}

Rect GridObject::getBounds() const {
	return level->getGrid().cellBounds((float) col, (float) row);
}
