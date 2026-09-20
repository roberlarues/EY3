#ifndef MAZE_SPRITE_H
#define MAZE_SPRITE_H

class SpriteSheet;

/**
 * The surface a GameObject is drawn with: the sheet it takes its texture
 * from and, when that sheet has several frames, which one it is showing.
 *
 * That state belongs to the object, not to the sheet: two objects drawn from
 * the same sheet keep their own frame and their own timing, and each decides
 * when its animation runs. Copying a Sprite is cheap -- a pointer and a
 * couple of numbers.
 *
 * It does not know how to draw itself: in 3D what is drawn is a mesh, and it
 * is MeshRenderer that puts the two together.
 */
class Sprite {
	private:
		const SpriteSheet* sheet;
		int frame;
		float frameRate;
		float frameTimer;

	public:
		Sprite();
		Sprite(const SpriteSheet* sheet);

		/** Frames per second; 0 (the default) holds the current frame. */
		void setFrameRate(float framesPerSecond);
		void setFrame(int frame);
		int getFrame() const;

		/** Advances the animation. Call it from the object's update(). */
		void update(float deltaTime);

		const SpriteSheet* getSheet() const;
};

#endif // MAZE_SPRITE_H
