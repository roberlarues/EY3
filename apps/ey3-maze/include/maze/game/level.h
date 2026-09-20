#ifndef MAZE_LEVEL_H
#define MAZE_LEVEL_H

#include "maze/game/grid.h"
#include "maze/game/scene.h"

/**
 * The scene the game is played on: its objects sit on a grid of cells, and
 * they can ask it what is where.
 *
 * It does not know how it was filled. Its cells come from a LevelData read
 * out of assets/levels/, and turning those characters into objects is the
 * game's job, so the same Level works for any grid game.
 */
class Level : public Scene {
	private:
		Grid grid;

	protected:
		void onInit();

	public:
		Level(SpriteLibrary& sprites, int cols, int rows);

		/** Whether a cell is inside the grid and no object blocks it. */
		bool isFree(int col, int row) const;

		const Grid& getGrid() const;
};

#endif // MAZE_LEVEL_H
