#ifndef MAZE_LEVEL_LIBRARY_H
#define MAZE_LEVEL_LIBRARY_H

#include <vector>

#include <ey3.h>

#include "maze/game/level_data.h"

using namespace ey3;

/**
 * The levels the game can play, read from assets/levels/.
 *
 * assets/levels/index.txt lists the level files, one per line and in the
 * order they are played, so adding a level is dropping a text file next to
 * the others and naming it there -- no rebuild, and nothing in the code
 * mentions any particular level.
 *
 * Reading them needs no GL context, only an AssetLoader, so the library can
 * be filled as soon as the app starts.
 */
class LevelLibrary {
	private:
		std::vector<LevelData> levels;

	public:
		/** Reads index.txt and every level file it lists. */
		void load(AssetLoader* assetLoader);

		int size() const;

		/** The level at an index, or an empty one if there is none. */
		const LevelData& get(int index) const;
};

#endif // MAZE_LEVEL_LIBRARY_H
