#ifndef EY3MAT4_H
#define EY3MAT4_H

namespace ey3 {

	/**
	 * A 4x4 matrix, stored the way GL wants it (column major), so data() can
	 * go straight into glUniformMatrix4fv without transposing.
	 *
	 * Only what a simple 3D app needs: the usual transforms and, above all,
	 * frustum(), which takes the four edges of the near plane separately and
	 * so can be asymmetric -- that is what head-coupled perspective needs.
	 */
	class Mat4 {
		private:
			float m[16];

		public:
			Mat4();   // identity

			static Mat4 translation(float x, float y, float z);
			static Mat4 scaling(float x, float y, float z);

			/** Rotation of angleRadians around the (x, y, z) axis. */
			static Mat4 rotation(float angleRadians, float x, float y, float z);

			/**
			 * Perspective from an off-centre window on the near plane. With
			 * left/right and bottom/top symmetric this is the usual
			 * perspective; asymmetric, it is the projection of an eye looking
			 * through a window that is not in front of it.
			 */
			static Mat4 frustum(float left, float right, float bottom, float top,
			                    float near, float far);

			Mat4 operator*(const Mat4& other) const;

			const float* data() const;
	};
}

#endif // EY3MAT4_H
