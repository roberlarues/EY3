#ifndef MAZE_WALL_H
#define MAZE_WALL_H

#include "maze/graphics/sprite.h"
#include "maze/objects/grid_object.h"

/** A wall: nothing may enter its cell. */
class Wall : public GridObject {
	private:
		Sprite sprite;

	public:
		Wall(Level* level, int col, int row);

		void onInit();
		void render();
		bool blocks(int col, int row) const;
};

#endif // MAZE_WALL_H
