#ifndef MAZE_PLAYER_H
#define MAZE_PLAYER_H

#include "maze/game/direction.h"
#include "maze/graphics/sprite.h"
#include "maze/objects/grid_object.h"

class DirectionSource;

/**
 * The ball: it rolls from cell to cell over whatever the level says is free,
 * sliding smoothly between them and rolling its sprite while it travels.
 *
 * It does not know how it is being steered. It reads a direction from the
 * DirectionSource it was given (the touch pad, in this game) and moves that
 * way while the direction holds; anything else that can answer "which way?"
 * drives it just as well.
 */
class Player : public GridObject {
	private:
		const DirectionSource* controls;

		// The cell it is rolling towards, and how far along it is.
		int targetCol;
		int targetRow;
		float moveProgress;   // 0.0 at the current cell, 1.0 at the target one
		bool moving;

		bool celebrating;
		Sprite ballSprite;
		Sprite winSprite;

		bool startMove(Direction direction);

	public:
		Player(Level* level, int col, int row, const DirectionSource* controls);

		void onInit();
		void update(float deltaTime);
		void render();

		/** Freezes the ball and swaps its sprite, for when the game is won. */
		void celebrate();
};

#endif // MAZE_PLAYER_H
