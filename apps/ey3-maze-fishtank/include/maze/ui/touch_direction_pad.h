#ifndef MAZE_TOUCH_DIRECTION_PAD_H
#define MAZE_TOUCH_DIRECTION_PAD_H

#include <ey3.h>

#include "maze/graphics/sprite.h"
#include "maze/objects/game_object.h"
#include "maze/game/direction_source.h"

using namespace ey3;

/**
 * The on-screen controls: it splits the screen into four triangular regions
 * around its center and reports the direction of the one being touched, for
 * as long as the touch is held, with an arrow at the middle of each screen
 * edge showing where to press (the one being held is drawn bigger).
 *
 * It is a GameObject like any other -- it draws itself and it listens to
 * input -- but it belongs to the interface scene rather than to the level: it
 * measures itself against the screen, knows nothing about grids or games, and
 * whatever wants to be driven by it reads it through DirectionSource.
 *
 * Its arrows are drawn on the plane of the screen (z = 0), where the
 * head-coupled projection leaves them exactly where they are however the
 * player moves -- the world shifts behind them, the controls do not.
 */
class TouchDirectionPad : public GameObject, public DirectionSource {
	private:
		Direction heldDirection;
		Sprite arrows[4];

	public:
		TouchDirectionPad(Scene* scene);

		void onInit();
		void render();

		/** Picks the direction from the region of the screen being touched. */
		void handleInput(const InputEvent& event);

		/** DirectionSource. */
		Direction getDirection() const;
};

#endif // MAZE_TOUCH_DIRECTION_PAD_H
