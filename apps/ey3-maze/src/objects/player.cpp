#include "maze/objects/player.h"

#include "maze/game/depths.h"
#include "maze/game/level.h"
#include "maze/game/direction_source.h"

// Cells per second. One cell is one tile, so this is also the speed in tiles.
static const float MOVE_SPEED = 5.0f;

// How fast the ball's four frames go round while it rolls. At this rate the
// highlight makes one turn per cell crossed, so it looks like it rolls.
static const float ROLL_FRAME_RATE = MOVE_SPEED * 4.0f;

Player::Player(Level* level, int col, int row, const DirectionSource* controls)
	: GridObject(level, col, row), controls(controls), targetCol(col), targetRow(row),
	  moveProgress(0.0f), moving(false), celebrating(false) {
}

void Player::onInit() {
	ballSprite = level->getSprites().create("sprites/ball.png");
	ballSprite.setFrameRate(ROLL_FRAME_RATE);
	winSprite = level->getSprites().create("sprites/ball_win.png");
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
		// The ball only rolls while it travels: standing still, it keeps the
		// frame it stopped at. Its animation is the player's business.
		ballSprite.update(deltaTime);

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
	Rect bounds = level->getGrid().cellBounds(col + (targetCol - col) * moveProgress,
	                                          row + (targetRow - row) * moveProgress);

	if (celebrating) {
		winSprite.draw(bounds, DEPTH_PLAYER);
	} else {
		ballSprite.draw(bounds, DEPTH_PLAYER);
	}
}

void Player::celebrate() {
	celebrating = true;
	moving = false;
	moveProgress = 0.0f;
	targetCol = col;
	targetRow = row;
}
