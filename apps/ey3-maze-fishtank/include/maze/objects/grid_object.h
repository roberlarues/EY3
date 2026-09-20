#ifndef MAZE_GRID_OBJECT_H
#define MAZE_GRID_OBJECT_H

#include <ey3.h>

#include "maze/objects/game_object.h"

class Level;

/**
 * A GameObject that occupies a cell of the level's grid: the walls, the
 * floor, the exit and the player are all of them.
 *
 * All it adds is the cell it sits on, where that cell is in the world (so
 * nothing has to redo the arithmetic) and a typed pointer to the level, which
 * is the scene it belongs to. It says nothing about how
 * the object looks or behaves -- that is each object's own business, and
 * screen-space things like the controls in maze/ui are plain GameObjects.
 */
class GridObject : public GameObject {
	protected:
		Level* level;
		int col;
		int row;

	public:
		GridObject(Level* level, int col, int row);

		int getCol() const;
		int getRow() const;

		/** Centre of this object's cell, on the floor of the box. */
		Vec3 getCellCenter() const;
};

#endif // MAZE_GRID_OBJECT_H
