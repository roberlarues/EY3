#ifndef MAZE_FISHTANK_VIEW_H
#define MAZE_FISHTANK_VIEW_H

#include <ey3.h>

using namespace ey3;

/**
 * The head-coupled perspective: it turns "where the player's eyes are" into
 * the matrix everything is drawn with.
 *
 * The screen is not a picture, it is a window. The eye is the apex of a
 * pyramid whose base is the screen rectangle, so when the player moves, the
 * pyramid becomes lopsided and the scene behind the window shifts the way a
 * real scene behind a real window would. That is why the projection is built
 * with an asymmetric frustum and the camera is never rotated: rotating it
 * instead is the usual mistake, and it shears the image at the edges.
 *
 * Everything is in centimetres, with the origin at the centre of the screen.
 */
class FishtankView {
	private:
		float screenWidthCm;
		float screenHeightCm;
		Vec3 eye;
		Mat4 viewProjection;

		void rebuild();

	public:
		FishtankView();

		void setScreenSize(float widthCm, float heightCm);
		void setEyePosition(const Vec3& eye);

		const Vec3& getEyePosition() const;
		const Mat4& getViewProjection() const;
};

#endif // MAZE_FISHTANK_VIEW_H
