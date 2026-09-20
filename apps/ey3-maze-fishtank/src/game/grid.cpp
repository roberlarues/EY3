#include "maze/game/grid.h"

#include <math.h>

// How much of the screen the maze takes. The floor is exactly one cell
// behind it, so the walls -- which are one cell tall -- have their top faces
// on the plane of the screen itself.
static const float MAZE_FILL = 0.86f;

// Distance the maze is sized for, in centimetres: it is the resting distance
// of HeadTracker, so from where a player normally sits the maze fills the
// screen. Without this the maze would look small, because it really is
// behind the screen and perspective shrinks it.
static const float REFERENCE_EYE_CM = 50.0f;

Grid::Grid(int cols, int rows)
	: cols(cols), rows(rows), cellSize(1.0f), originX(0.0f), originY(0.0f), floorZ(-10.0f) {
}

void Grid::fit(float screenWidthCm, float screenHeightCm) {
	// The cell that would fill the screen if the maze were painted on it,
	// then the cell grown by as much as one cell of depth shrinks it. Solving
	// cell = flat * (ref + cell) / ref for cell gives this, and it puts the
	// tops of the walls exactly on z = 0 -- the plane of the screen.
	//
	// That is what makes the phone read as hollow: anything at z = 0 projects
	// to the same pixels whatever the eye is doing, so the tops of the walls
	// never move, and all the parallax happens in the corridors sinking away
	// behind them.
	float flatCell = fminf(screenWidthCm / cols, screenHeightCm / rows) * MAZE_FILL;
	float room = REFERENCE_EYE_CM - flatCell;
	cellSize = room > 1.0f ? flatCell * REFERENCE_EYE_CM / room : flatCell;
	floorZ = -cellSize;

	// Centre of cell (0, 0): half a maze to the left and up from the middle.
	originX = -(cols - 1) * cellSize / 2.0f;
	originY = (rows - 1) * cellSize / 2.0f;

	LOGI("Grid: %dx%d cells of %.2f cm, floor %.2f cm behind the screen "
	     "(wall tops flush with it)", cols, rows, cellSize, -floorZ);
}

int Grid::getCols() const {
	return cols;
}

int Grid::getRows() const {
	return rows;
}

bool Grid::contains(int col, int row) const {
	return col >= 0 && row >= 0 && col < cols && row < rows;
}

float Grid::getCellSize() const {
	return cellSize;
}

float Grid::getFloorZ() const {
	return floorZ;
}

Vec3 Grid::getCellCenter(float col, float row) const {
	// Rows count downwards on the map and upwards in the world.
	return Vec3{originX + col * cellSize, originY - row * cellSize, floorZ};
}
