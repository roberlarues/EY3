#ifndef MAZE_EXIT_H
#define MAZE_EXIT_H

#include "maze/graphics/sprite.h"
#include "maze/objects/grid_object.h"

/**
 * The way out. Walkable, and it breathes: its sheet has several frames and
 * it runs through them on its own. What reaching it means is up to the game
 * rules, which only ask where it is.
 */
class Exit : public GridObject {
	private:
		Sprite sprite;

	public:
		Exit(Level* level, int col, int row);

		void onInit();
		void update(float deltaTime);
		void render();
};

#endif // MAZE_EXIT_H
