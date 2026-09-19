#ifndef MAZE_GRID_OBJECT_H
#define MAZE_GRID_OBJECT_H

#include "maze/graphics/rect.h"
#include "maze/objects/game_object.h"

class Level;

/**
 * A GameObject that occupies a cell of the level's grid: the walls, the
 * floor, the exit and the player are all of them.
 *
 * All it adds is the cell it sits on, the rectangle that cell takes on screen
 * (so nothing has to redo the cell-to-pixels arithmetic) and a typed pointer
 * to the level, which is the scene it belongs to. It says nothing about how
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

		/** The rectangle this object's cell takes on screen. */
		Rect getBounds() const;
};

#endif // MAZE_GRID_OBJECT_H
