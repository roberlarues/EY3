#ifndef EY3VEC3_H
#define EY3VEC3_H

namespace ey3 {

	/**
	 * A point or direction in 3D. Plain data: the engine only needs it to
	 * carry positions around (see HeadTracker) and to build matrices.
	 */
	struct Vec3 {
		float x;
		float y;
		float z;
	};
}

#endif // EY3VEC3_H
