#ifndef MAZE_MAZE_GAME_H
#define MAZE_MAZE_GAME_H

#include <ey3.h>

#include "maze/game/fishtank_view.h"
#include "maze/game/level.h"
#include "maze/game/level_library.h"
#include "maze/game/scene.h"
#include "maze/graphics/mesh_renderer.h"
#include "maze/graphics/sprite_library.h"

using namespace ey3;

class Exit;
class Player;
class TouchDirectionPad;

/**
 * The rules of the game, and what holds the pieces together: it takes the
 * levels from the LevelLibrary, turns each one's characters into objects, and
 * every frame lets them live their own lives and checks whether the player
 * has reached the exit.
 *
 * It also owns the viewpoint. Every frame it asks the front camera where the
 * player's eyes are (HeadTracker) and hands that to the FishtankView, which
 * turns it into the matrix everything is drawn with. With no camera -- a
 * desktop without a webcam, a phone that refused the permission -- it falls
 * back to moving the viewpoint by itself, so the effect can still be seen.
 *
 * It contains no level: the maps live in assets/levels/. It keeps two scenes
 * apart, the Level being played and the interface, and it owns the art and
 * the geometry both of them share.
 */
class MazeGame {
	private:
		AssetLoader assetLoader;   // reading assets needs no GL context
		SpriteLibrary sprites;     // shared by both scenes and every level
		MeshRenderer meshes;       // the shapes and the shader, shared too
		LevelLibrary levels;

		Scene hud;                       // the interface; lasts the whole game
		TouchDirectionPad* controls;     // owned by the interface scene
		Level* level;                    // the level being played
		Player* player;                  // owned by the level
		Exit* exit;                      // owned by the level

		int levelIndex;
		bool levelFinished;
		float winTimer;

		// The viewpoint and where it comes from.
		Camera camera;
		HeadTracker tracker;
		FishtankView view;
		cv::Mat frame;
		bool cameraEnabled;
		bool cameraGivenUp;
		bool calibrated;
		float timeWithoutCamera;
		float demoTime;

		void buildLevel();
		void updateViewpoint(float deltaTime);

		/** Tells the tracker about this device, once it can be asked. */
		void calibrateTracker();

	public:
		MazeGame();
		~MazeGame();

		Level& getLevel();

		/** The interface layer: the controls, and whatever is added later. */
		Scene& getHud();

		/** The front camera, for the program to hook to the window's lifecycle. */
		Camera& getCamera();

		/** One frame: the viewpoint, then everything moves, then the rules. */
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
