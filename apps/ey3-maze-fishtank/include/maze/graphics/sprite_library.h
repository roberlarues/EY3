#ifndef MAZE_SPRITE_LIBRARY_H
#define MAZE_SPRITE_LIBRARY_H

#include <map>
#include <string>

#include <ey3.h>

#include "maze/graphics/sprite.h"

using namespace ey3;

class SpriteSheet;

/**
 * Loads sprite sheets and keeps one per asset path, so asking twice for the
 * same PNG gives two Sprites sharing a single texture. This is what keeps the
 * maze's ~200 walls and floor tiles down to one upload each.
 */
class SpriteLibrary {
	private:
		std::map<std::string, SpriteSheet*> sheets;
		AssetLoader* assetLoader;
		bool ready;
		int32_t contextGeneration;

	public:
		SpriteLibrary();
		~SpriteLibrary();

		/**
		 * Called whenever a scene is initialized. When that means a new GL
		 * context -- which on Android happens every time the app comes back
		 * from the background, with every texture gone -- the sheets already
		 * handed out are uploaded again **in place**, so the Sprites
		 * pointing at them keep working. Called twice for the same context
		 * (one scene each) it does the work once.
		 */
		void init(Renderer* renderer, AssetLoader* assetLoader);

		/** A Sprite drawing the sheet at assetPath, loading it if needed. */
		Sprite create(const char* assetPath);
};

#endif // MAZE_SPRITE_LIBRARY_H
