#ifndef MAZE_SPRITE_SHEET_H
#define MAZE_SPRITE_SHEET_H

#include <ey3.h>

using namespace ey3;

/**
 * A sprite sheet loaded from assets/sprites/: a PNG holding a horizontal
 * strip of square frames, so a 16x16 image is one still frame and a 64x16 one
 * is a four frame animation -- the frame count is the image's width divided
 * by its height, there is nothing else to declare.
 *
 * Sheets are shared: the texture is uploaded once and any number of Sprites
 * can draw from it (see SpriteLibrary).
 *
 * The images carry no alpha channel (AssetLoader::loadImageAsset reads them
 * as BGR), so transparency is done with a color key: texels painted pure
 * magenta are discarded by assets/shaders/sprite.frag.
 */
class SpriteSheet {
	private:
		Texture texture;
		int frameCount;

	public:
		SpriteSheet();

		/** Loads and uploads the PNG. Needs a current GL context. */
		bool load(AssetLoader* assetLoader, const char* assetPath);

		void bind() const;
		int getFrameCount() const;
};

#endif // MAZE_SPRITE_SHEET_H
