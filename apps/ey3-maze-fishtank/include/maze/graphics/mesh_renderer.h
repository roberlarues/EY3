#ifndef MAZE_MESH_RENDERER_H
#define MAZE_MESH_RENDERER_H

#include <ey3.h>

#include "maze/graphics/mesh.h"
#include "maze/graphics/sprite.h"

using namespace ey3;

/**
 * Draws the game's geometry: it owns the shader, the three shapes everything
 * in this maze is made of, and the view-projection matrix they are all seen
 * through.
 *
 * That matrix is the whole point of this app -- it comes from FishtankView,
 * which builds it from where the player's eyes are, so the same scene looks
 * different depending on where the player is sitting. Everything else here is
 * ordinary: a mesh, a texture and a model matrix.
 *
 * One instance is shared by every scene, like the SpriteLibrary is.
 */
class MeshRenderer {
	private:
		Shader shader;
		Mesh planeMesh;
		Mesh cubeMesh;
		Mesh sphereMesh;

		Mat4 viewProjection;
		GLint modelLocation;
		GLint viewProjectionLocation;
		GLint spriteFrameLocation;
		GLint tintLocation;
		bool ready;
		int32_t contextGeneration;

		void draw(const Mesh& mesh, const Sprite& sprite, const Mat4& model, const Vec3& tint);

	public:
		MeshRenderer();

		/**
		 * Compiles the shader and builds the shapes. Called whenever a scene
		 * is initialized, and it rebuilds them when that means a new GL
		 * context -- on Android the app coming back from the background
		 * leaves neither shader nor meshes alive.
		 */
		void init(Renderer* renderer, AssetLoader* assetLoader);

		/** How the world is seen this frame. Set it once, before drawing. */
		void setViewProjection(const Mat4& viewProjection);

		void drawPlane(const Sprite& sprite, const Mat4& model,
		               const Vec3& tint = Vec3{1.0f, 1.0f, 1.0f});
		void drawCube(const Sprite& sprite, const Mat4& model,
		              const Vec3& tint = Vec3{1.0f, 1.0f, 1.0f});
		void drawSphere(const Sprite& sprite, const Mat4& model,
		                const Vec3& tint = Vec3{1.0f, 1.0f, 1.0f});
};

#endif // MAZE_MESH_RENDERER_H
