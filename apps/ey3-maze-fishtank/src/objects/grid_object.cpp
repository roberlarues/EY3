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

Vec3 GridObject::getCellCenter() const {
	return level->getGrid().getCellCenter((float) col, (float) row);
}
