#ifndef MAZE_MESH_H
#define MAZE_MESH_H

#include <ey3.h>

using namespace ey3;

/**
 * A piece of geometry on the GPU: position, normal and texture coordinates
 * per vertex, ready to be drawn.
 *
 * The three shapes this game is made of are built in code rather than loaded
 * from a file -- a cube, a plane and a sphere are a handful of lines each,
 * and a mesh format would be more machinery than the game needs.
 */
class Mesh {
	private:
		GLuint vao;
		int vertexCount;

	public:
		Mesh();

		/** Uploads interleaved vertices (3 position, 3 normal, 2 uv). */
		void load(const float* vertices, int vertexCount);
		void draw() const;
		bool isLoaded() const;

		/** A 1x1 square on the XY plane, facing +z. */
		static Mesh plane();

		/** A 1x1x1 cube centred on the origin. */
		static Mesh cube();

		/** A sphere of diameter 1 centred on the origin. */
		static Mesh sphere(int rings, int sectors);
};

#endif // MAZE_MESH_H
