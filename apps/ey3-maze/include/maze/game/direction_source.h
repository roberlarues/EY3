#ifndef MAZE_DIRECTION_SOURCE_H
#define MAZE_DIRECTION_SOURCE_H

#include "maze/game/direction.h"

/**
 * Where something that moves takes the direction it is being asked to go in.
 *
 * This is the seam between the game and whatever drives it, and it lives on
 * the game's side on purpose: the player holds one of these and never learns
 * whether the direction came from a touch, a key, a swipe or a script
 * replaying a solution, and nothing under maze/game depends on maze/ui.
 */
class DirectionSource {
	public:
		virtual ~DirectionSource() {}

		/** The direction being asked for right now, or NONE. */
		virtual Direction getDirection() const = 0;
};

#endif // MAZE_DIRECTION_SOURCE_H
