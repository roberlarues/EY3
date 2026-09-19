#ifndef MAZE_GAME_OBJECT_H
#define MAZE_GAME_OBJECT_H

#include <ey3.h>

using namespace ey3;

class Scene;

/**
 * Anything that takes part in a Scene: a wall, a floor cell, the player, a
 * widget of the interface.
 *
 * Each one is an ey3 Renderizable, and its Scene is the composite that drives
 * them all: it initializes, draws and feeds input to each object. On top of
 * that a GameObject can run its own logic every frame and block the cell it
 * occupies. Whatever an object does is its own business: the game rules
 * (MazeGame) don't need to know how the player moves, and the player doesn't
 * need to know what a wall is.
 */
class GameObject : public Renderizable {
	protected:
		Scene* scene;

	public:
		GameObject(Scene* scene);
		virtual ~GameObject();

		/**
		 * Renderizable: called by the scene once the GL context exists and
		 * its shared resources are ready. Objects don't need the arguments,
		 * so it just forwards to onInit().
		 */
		void init(Renderer* renderer, AssetLoader* assetLoader);

		/**
		 * Where an object takes the sprites it draws itself with, out of
		 * scene->getSprites(). Called once, with the scene's shared
		 * resources ready and a GL context current.
		 */
		virtual void onInit();

		/** Renderizable: draw yourself, every frame. */
		virtual void render() = 0;

		/** This object's own logic, run once per frame. Nothing by default. */
		virtual void update(float deltaTime);

		/**
		 * Whether this object keeps others out of a cell. The Level asks
		 * every object it holds, so each kind decides what it blocks.
		 */
		virtual bool blocks(int col, int row) const;

		/**
		 * Input the scene passes on, for the objects that care (here, only
		 * the controls do). Ignored by default.
		 */
		virtual void handleInput(const InputEvent& event);
};

#endif // MAZE_GAME_OBJECT_H
