#include "maze/objects/player.h"

#include "maze/game/level.h"
#include "maze/game/direction_source.h"

// Cells per second. One cell is one tile, so this is also the speed in tiles.
static const float MOVE_SPEED = 5.0f;

// Size of the ball as a share of a cell. Small enough to fit through a gap
// with room to spare, big enough to be the thing you look at.
static const float BALL_SIZE = 0.72f;

Player::Player(Level* level, int col, int row, const DirectionSource* controls)
	: GridObject(level, col, row), controls(controls), targetCol(col), targetRow(row),
	  moveProgress(0.0f), moving(false), celebrating(false) {
}

void Player::onInit() {
	ballSprite = level->getSprites().create("sprites/ball3d.png");
	winSprite = level->getSprites().create("sprites/ball3d_win.png");
}

bool Player::startMove(Direction direction) {
	if (direction == Direction::NONE || celebrating) {
		return false;
	}

	int nextCol = col + colOffset(direction);
	int nextRow = row + rowOffset(direction);
	if (!level->isFree(nextCol, nextRow)) {
		return false;
	}

	targetCol = nextCol;
	targetRow = nextRow;
	moving = true;
	return true;
}

void Player::update(float deltaTime) {
	Direction asked = controls->getDirection();

	if (moving) {
		// Turn the ball by the distance it covers: angle = distance / radius.
		// The axis is perpendicular to where it is going, which is what makes
		// it look like it grips the floor rather than spinning on the spot.
		float cellSize = level->getGrid().getCellSize();
		float radius = cellSize * BALL_SIZE / 2.0f;
		if (radius > 0.0f) {
			float distance = MOVE_SPEED * deltaTime * cellSize;
			float dirX = (float) (targetCol - col);
			float dirY = -(float) (targetRow - row);   // rows go down, the world goes up
			roll = Mat4::rotation(distance / radius, -dirY, dirX, 0.0f) * roll;
		}

		moveProgress += MOVE_SPEED * deltaTime;

		// A frame can be long enough to cross more than one cell, so keep
		// spending the leftover progress until the ball stops or is mid-cell.
		while (moveProgress >= 1.0f) {
			moveProgress -= 1.0f;
			col = targetCol;
			row = targetRow;

			if (!startMove(asked)) {
				moveProgress = 0.0f;
				moving = false;
				break;
			}
		}
	}

	// Standing still on a cell: start rolling as soon as a direction is being
	// asked for and the cell that way is free.
	if (!moving) {
		startMove(asked);
	}
}

void Player::render() {
	// Interpolated between the cell it left and the one it is heading to, so
	// the ball slides instead of jumping from cell to cell.
	const Grid& grid = level->getGrid();
	Vec3 centre = grid.getCellCenter(col + (targetCol - col) * moveProgress,
	                                 row + (targetRow - row) * moveProgress);

	float diameter = grid.getCellSize() * BALL_SIZE;
	Mat4 model = Mat4::translation(centre.x, centre.y, centre.z + diameter / 2.0f)
		* roll
		* Mat4::scaling(diameter, diameter, diameter);

	level->getMeshes().drawSphere(celebrating ? winSprite : ballSprite, model);
}

void Player::celebrate() {
	celebrating = true;
	moving = false;
	moveProgress = 0.0f;
	targetCol = col;
	targetRow = row;
}
