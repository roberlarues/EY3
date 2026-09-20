#ifndef MAZE_SPRITE_H
#define MAZE_SPRITE_H

#include "maze/graphics/rect.h"

class SpriteSheet;
class SpriteRenderer;

/**
 * How a GameObject knows to draw itself: the sheet it draws from plus, when
 * that sheet has several frames, which one it is showing right now.
 *
 * That state belongs to the object, not to the sheet: two objects drawn from
 * the same sheet keep their own frame and their own timing, and each decides
 * when its animation runs (the player only rolls while it is moving). Copying
 * a Sprite is cheap -- it is a pointer to the shared sheet and a couple of
 * numbers.
 */
class Sprite {
	private:
		const SpriteSheet* sheet;
		SpriteRenderer* renderer;
		int frame;
		float frameRate;
		float frameTimer;

	public:
		Sprite();
		Sprite(const SpriteSheet* sheet, SpriteRenderer* renderer);

		/** Frames per second; 0 (the default) holds the current frame. */
		void setFrameRate(float framesPerSecond);
		void setFrame(int frame);
		int getFrame() const;

		/** Advances the animation. Call it from the object's update(). */
		void update(float deltaTime);

		void draw(const Rect& bounds, float depth) const;
};

#endif // MAZE_SPRITE_H
