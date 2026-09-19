#ifndef MAZE_MAZE_GAME_H
#define MAZE_MAZE_GAME_H

#include <ey3.h>

#include "maze/game/level.h"
#include "maze/game/level_library.h"
#include "maze/game/scene.h"
#include "maze/graphics/sprite_library.h"

using namespace ey3;

class Exit;
class Player;
class TouchDirectionPad;

/**
 * The rules of the game, and what holds the pieces together: it takes the
 * levels from the LevelLibrary, turns each one's characters into objects, and
 * every frame lets them live their own lives and checks whether the player
 * has reached the exit -- at which point the ball celebrates for a moment and
 * the level is over.
 *
 * It contains no level: the maps live in assets/levels/ and nothing here
 * mentions any of them. It keeps two scenes apart, the Level being played
 * (thrown away and rebuilt for the next one) and the interface, which lasts
 * the whole game, and it owns the art both of them share.
 *
 * It is deliberately not a Renderizable, and it never touches the engine: the
 * program takes the objects of these scenes and registers them (see
 * src/program.cpp).
 */
class MazeGame {
	private:
		AssetLoader assetLoader;   // reading assets needs no GL context
		SpriteLibrary sprites;     // shared by both scenes and every level
		LevelLibrary levels;

		Scene hud;                       // the interface; lasts the whole game
		TouchDirectionPad* controls;     // owned by the interface scene
		Level* level;                    // the level being played
		Player* player;                  // owned by the level
		Exit* exit;                      // owned by the level

		int levelIndex;
		bool levelFinished;
		float winTimer;

		void buildLevel();

	public:
		MazeGame();
		~MazeGame();

		Level& getLevel();
		/** The interface layer: the controls, and whatever is added later. */
		Scene& getHud();

		/** One frame: everything moves, then the rules are applied. */
		void update(float deltaTime);

		/** Whether the level has been cleared and can be swapped out. */
		bool isLevelFinished() const;

		/**
		 * Throws away the level being played and builds the next one, wrapping
		 * around at the end. Its objects must already be out of the engine.
		 */
		void nextLevel();
};

#endif // MAZE_MAZE_GAME_H
