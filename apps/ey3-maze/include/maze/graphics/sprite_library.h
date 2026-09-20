#ifndef MAZE_SPRITE_LIBRARY_H
#define MAZE_SPRITE_LIBRARY_H

#include <map>
#include <string>

#include <ey3.h>

#include "maze/graphics/sprite.h"
#include "maze/graphics/sprite_renderer.h"

using namespace ey3;

class SpriteSheet;

/**
 * Loads sprite sheets and keeps one per asset path, so asking twice for the
 * same PNG gives two Sprites sharing a single texture. This is what keeps the
 * maze's ~200 wall and floor tiles down to one upload each.
 */
class SpriteLibrary {
	private:
		SpriteRenderer renderer;
		std::map<std::string, SpriteSheet*> sheets;
		AssetLoader* assetLoader;
		bool ready;
		int32_t contextGeneration;

	public:
		SpriteLibrary();
		~SpriteLibrary();

		/** Prepares the shared renderer. Needs a current GL context. */
		void init(Renderer* renderer, AssetLoader* assetLoader);

		/** A Sprite drawing the sheet at assetPath, loading it if needed. */
		Sprite create(const char* assetPath);
};

#endif // MAZE_SPRITE_LIBRARY_H
