#ifndef MAZE_DIRECTION_H
#define MAZE_DIRECTION_H

/** A step from one cell of a grid to one of its four neighbours. */
enum class Direction {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

/** How a step in this direction changes a cell's column. */
int colOffset(Direction direction);

/** How a step in this direction changes a cell's row. */
int rowOffset(Direction direction);

#endif // MAZE_DIRECTION_H
