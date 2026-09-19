#ifndef MAZE_SPRITE_RENDERER_H
#define MAZE_SPRITE_RENDERER_H

#include <ey3.h>

#include "maze/graphics/rect.h"

using namespace ey3;

class SpriteSheet;

/**
 * Draws one frame of a sheet as a screen-space quad: owns the shader and the
 * unit quad every frame is stretched onto. There is one per SpriteLibrary,
 * shared by every Sprite that comes out of it.
 */
class SpriteRenderer {
	private:
		Shader shader;
		GLuint vao;
		GLint screenSizeLocation;
		GLint spriteRectLocation;
		GLint spriteFrameLocation;
		GLint depthLocation;
		float screenWidth;
		float screenHeight;

	public:
		SpriteRenderer();
		void init(AssetLoader* assetLoader, int32_t screenWidth, int32_t screenHeight);

		/**
		 * Draws a frame into a pixel rectangle. depth is NDC (-1 nearest,
		 * 1 farthest): the engine's Renderer keeps GL_DEPTH_TEST on, so
		 * sprites that overlap need different depths.
		 */
		void draw(const SpriteSheet& sheet, int frame, const Rect& bounds, float depth);
};

#endif // MAZE_SPRITE_RENDERER_H
