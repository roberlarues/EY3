#ifndef MAZE_GRID_H
#define MAZE_GRID_H

#include "maze/graphics/rect.h"

/**
 * Size, in pixels, of one frame of the tile art in assets/sprites/. The grid
 * scales it up by whole numbers so the pixels stay square and sharp.
 */
const int TILE_ART_SIZE = 16;

/**
 * Where each cell of a level lands on the screen.
 *
 * The tile art is scaled by a whole number of pixels (so every texel covers
 * the same square of screen pixels and the art stays sharp), picking the
 * largest scale whose grid still fits, and the result is centered.
 */
class Grid {
	private:
		int cols;
		int rows;
		float tileSize;
		float originX;
		float originY;
		float screenWidth;
		float screenHeight;

	public:
		Grid(int cols, int rows);

		/** Recomputes the layout; the surface size is known once GL is up. */
		void fit(float screenWidth, float screenHeight);

		int getCols() const;
		int getRows() const;
		bool contains(int col, int row) const;

		float getTileSize() const;
		float getScreenWidth() const;
		float getScreenHeight() const;

		/**
		 * Where a cell lands on screen. Fractional cells are allowed, which
		 * is how the player draws itself while sliding between two of them.
		 */
		Rect cellBounds(float col, float row) const;
};

#endif // MAZE_GRID_H
