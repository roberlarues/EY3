#ifndef MAZE_SCENE_H
#define MAZE_SCENE_H

#include <vector>

#include <ey3.h>

#include "maze/graphics/mesh_renderer.h"
#include "maze/graphics/sprite_library.h"

using namespace ey3;

class GameObject;

/**
 * A layer of the game: the objects that live and are drawn together, plus
 * what they all share -- the sprite library and the mesh renderer they are
 * drawn with, and the size of the surface they are drawn on.
 *
 * A scene is itself an ey3 Renderizable, and so is every object in it: the
 * scene is the composite, and all it does is orchestrate the individual ones,
 * initializing, drawing and feeding input to each. That is what the program
 * hands to the engine, one registration per layer, so the objects inside can
 * come and go -- a whole level at a time -- without the engine noticing.
 *
 * A game has one scene per layer. This one has two: the Level, which is the
 * world and is thrown away and rebuilt on every level, and the interface,
 * which holds the controls and lasts the whole game.
 *
 * The scene owns its objects and deletes them. The sprite library is not
 * owned: it is shared with the other scenes, so art loaded for one level is
 * still there for the next.
 */
class Scene : public Renderizable, public InputListener {
	private:
		SpriteLibrary& sprites;
		MeshRenderer& meshes;
		std::vector<GameObject*> objects;

		// Kept from init() so objects added to a running scene can be
		// initialized as they arrive, the way the engine's Renderer does.
		Renderer* renderer;
		AssetLoader* assetLoader;

		float screenWidth;
		float screenHeight;
		float screenWidthCm;
		float screenHeightCm;
		bool initialized;

	protected:
		const std::vector<GameObject*>& getObjects() const;

		/** What this kind of scene sets up once the surface size is known. */
		virtual void onInit();

	public:
		Scene(SpriteLibrary& sprites, MeshRenderer& meshes);
		virtual ~Scene();

		/** Adds an object to the scene, taking ownership of it. */
		void add(GameObject* object);

		/** Runs every object's own logic. */
		void update(float deltaTime);

		/** Renderizable: prepares what is shared, then initializes its objects. */
		void init(Renderer* renderer, AssetLoader* assetLoader);

		/** Renderizable: draws every object. */
		void render();

		/** InputListener: hands the event to every object. */
		void handleInput(const InputEvent& event);

		SpriteLibrary& getSprites();
		MeshRenderer& getMeshes();

		float getScreenWidth() const;
		float getScreenHeight() const;

		/**
		 * Size of the surface in centimetres, which is what a scene drawn in
		 * real world units needs. Falls back to a 96 dpi guess when the
		 * platform cannot tell (see IWindow::getPhysicalWidthMm).
		 */
		float getScreenWidthCm() const;
		float getScreenHeightCm() const;
};

#endif // MAZE_SCENE_H
