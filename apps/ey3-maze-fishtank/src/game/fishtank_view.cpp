#include "maze/game/fishtank_view.h"

#include <math.h>

// The near plane is kept at half the distance to the eye: always in front of
// the screen, so nothing is ever clipped, and close enough to the scene that
// the depth buffer keeps its precision.
static const float NEAR_RATIO = 0.5f;
static const float FAR_DISTANCE_CM = 500.0f;

// However close the player leans in, the eye is never allowed nearer than
// this, which keeps the frustum sane.
static const float MIN_EYE_DISTANCE_CM = 5.0f;

FishtankView::FishtankView()
	: screenWidthCm(15.0f), screenHeightCm(25.0f), eye{0.0f, 0.0f, 50.0f} {
	rebuild();
}

void FishtankView::setScreenSize(float widthCm, float heightCm) {
	if (widthCm <= 0.0f || heightCm <= 0.0f) {
		return;
	}
	screenWidthCm = widthCm;
	screenHeightCm = heightCm;
	rebuild();
}

void FishtankView::setEyePosition(const Vec3& eye) {
	this->eye = eye;
	rebuild();
}

void FishtankView::rebuild() {
	float distance = fmaxf(eye.z, MIN_EYE_DISTANCE_CM);
	float near = distance * NEAR_RATIO;
	float far = distance + FAR_DISTANCE_CM;

	// The four edges of the screen, seen from the eye, projected onto the
	// near plane. The eye being off to one side is what tilts the pyramid.
	float scale = near / distance;
	float left = (-screenWidthCm / 2.0f - eye.x) * scale;
	float right = (screenWidthCm / 2.0f - eye.x) * scale;
	float bottom = (-screenHeightCm / 2.0f - eye.y) * scale;
	float top = (screenHeightCm / 2.0f - eye.y) * scale;

	viewProjection = Mat4::frustum(left, right, bottom, top, near, far)
		* Mat4::translation(-eye.x, -eye.y, -distance);
}

const Vec3& FishtankView::getEyePosition() const {
	return eye;
}

const Mat4& FishtankView::getViewProjection() const {
	return viewProjection;
}
