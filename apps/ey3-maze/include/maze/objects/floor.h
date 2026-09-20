#ifndef MAZE_FLOOR_H
#define MAZE_FLOOR_H

#include "maze/graphics/sprite.h"
#include "maze/objects/grid_object.h"

/** Plain ground: a cell the player can stand on, and nothing more. */
class Floor : public GridObject {
	private:
		Sprite sprite;

	public:
		Floor(Level* level, int col, int row);

		void onInit();
		void render();
};

#endif // MAZE_FLOOR_H
