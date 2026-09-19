#ifndef MAZE_RECT_H
#define MAZE_RECT_H

/**
 * A rectangle on screen, in pixels, with (x, y) at its top-left corner --
 * the same axes as touch events, y growing downwards.
 *
 * It is what anything that draws hands to a Sprite, so an object placed on
 * the grid and a widget placed against a screen edge say where they are in
 * the same terms.
 */
struct Rect {
	float x;
	float y;
	float width;
	float height;
};

#endif // MAZE_RECT_H
