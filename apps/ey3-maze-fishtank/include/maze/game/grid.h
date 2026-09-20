#ifndef MAZE_GRID_H
#define MAZE_GRID_H

#include <ey3.h>

using namespace ey3;

/**
 * Where each cell of a level sits in the world, in centimetres.
 *
 * Real units, not pixels: the whole point of this app is that the screen is a
 * window onto a box, and a window has a physical size. The maze is laid out
 * to fill the screen and pushed back behind it, so the player looks into it.
 *
 * The origin is the centre of the screen, with +x to the right, +y up and +z
 * towards the player, which is the same frame HeadTracker reports eyes in.
 */
class Grid {
	private:
		int cols;
		int rows;
		float cellSize;
		float originX;
		float originY;
		float floorZ;

	public:
		Grid(int cols, int rows);

		/** Lays the maze out for a screen of this size, in centimetres. */
		void fit(float screenWidthCm, float screenHeightCm);

		int getCols() const;
		int getRows() const;
		bool contains(int col, int row) const;

		/** Side of a cell, in centimetres. */
		float getCellSize() const;

		/** Depth of the floor of the box: negative, behind the screen. */
		float getFloorZ() const;

		/**
		 * Centre of a cell, on the floor of the box. Fractional cells are
		 * allowed, which is how the ball sits between two of them.
		 */
		Vec3 getCellCenter(float col, float row) const;
};

#endif // MAZE_GRID_H
