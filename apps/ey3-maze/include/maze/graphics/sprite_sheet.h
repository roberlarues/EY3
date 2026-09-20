#ifndef MAZE_SPRITE_SHEET_H
#define MAZE_SPRITE_SHEET_H

#include <string>

#include <ey3.h>

using namespace ey3;

/**
 * A sprite sheet loaded from assets/sprites/: a PNG holding a horizontal
 * strip of square frames, so a 16x16 image is one still frame and a 64x16 one
 * is a four frame animation -- the frame count is the image's width divided
 * by its height, there is nothing else to declare.
 *
 * Sheets are shared: the texture is uploaded once and any number of Sprites
 * can draw from it (see SpriteLibrary). A sheet remembers the asset it came
 * from, because on Android the GL context is destroyed whenever the app goes
 * to the background and every texture has to be uploaded again on the way
 * back (see SpriteLibrary::init).
 *
 * Transparency is the image's own alpha channel: loadImageAsset keeps it and
 * the sheet is uploaded as RGBA when the file has one.
 */
class SpriteSheet {
	private:
		Texture texture;
		std::string assetPath;
		int frameCount;

	public:
		SpriteSheet();

		/** Loads and uploads the PNG. Needs a current GL context. */
		bool load(AssetLoader* assetLoader, const char* assetPath);

		/** Uploads it again, from the same asset, onto a fresh GL context. */
		bool reload(AssetLoader* assetLoader);

		/** The GL context that held this sheet is gone: forget its texture. */
		void invalidate();

		void bind() const;
		int getFrameCount() const;
};

#endif // MAZE_SPRITE_SHEET_H
