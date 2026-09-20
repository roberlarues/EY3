#include "maze/ui/touch_direction_pad.h"

#include <math.h>

#include "maze/game/scene.h"

// Size of the arrows and their margin, as a share of the screen's shorter
// side, plus how much bigger the held one is drawn.
static const float ARROW_SIZE = 0.10f;
static const float ARROW_HELD_SCALE = 1.4f;
static const float ARROW_MARGIN = 0.03f;

// A hair in front of the screen. The tops of the walls are exactly on z = 0
// now, and two things at the same depth fight over which is drawn. At this
// distance the arrows still do not move with the viewpoint in any way you
// could see: a 10 cm move of the head shifts them by two hundredths of a
// millimetre.
static const float ARROW_Z_CM = 0.1f;

// Dim until pressed, so the controls stay out of the way of the illusion.
static const Vec3 ARROW_TINT = {0.65f, 0.65f, 0.7f};
static const Vec3 ARROW_HELD_TINT = {1.0f, 1.0f, 1.0f};

// The four arrows, and the directions they point at, in the same order.
static const Direction ARROW_DIRECTIONS[] = {
	Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT
};
static const char* const ARROW_SPRITES[] = {
	"sprites/arrow_up.png", "sprites/arrow_right.png",
	"sprites/arrow_down.png", "sprites/arrow_left.png"
};

TouchDirectionPad::TouchDirectionPad(Scene* scene)
	: GameObject(scene), heldDirection(Direction::NONE) {
}

void TouchDirectionPad::onInit() {
	for (int i = 0; i < 4; i++) {
		arrows[i] = scene->getSprites().create(ARROW_SPRITES[i]);
	}
}

void TouchDirectionPad::render() {
	float width = scene->getScreenWidthCm();
	float height = scene->getScreenHeightCm();
	float unit = fminf(width, height);
	float margin = unit * ARROW_MARGIN;

	for (int i = 0; i < 4; i++) {
		Direction direction = ARROW_DIRECTIONS[i];
		bool held = direction == heldDirection;
		float size = unit * ARROW_SIZE * (held ? ARROW_HELD_SCALE : 1.0f);

		float x = 0.0f;
		float y = 0.0f;
		if (direction == Direction::UP) {
			y = height / 2.0f - margin - size / 2.0f;
		} else if (direction == Direction::DOWN) {
			y = -height / 2.0f + margin + size / 2.0f;
		} else if (direction == Direction::LEFT) {
			x = -width / 2.0f + margin + size / 2.0f;
		} else {
			x = width / 2.0f - margin - size / 2.0f;
		}

		// Drawn on the plane of the screen itself. Anything at z = 0 lands on
		// the same pixels whatever the projection is doing, which is exactly
		// what interface elements want: they stay put while the world moves.
		Mat4 model = Mat4::translation(x, y, ARROW_Z_CM) * Mat4::scaling(size, size, 1.0f);
		scene->getMeshes().drawPlane(arrows[i], model, held ? ARROW_HELD_TINT : ARROW_TINT);
	}
}

void TouchDirectionPad::handleInput(const InputEvent& event) {
	if (event.type == InputEventType::POINTER_UP) {
		heldDirection = Direction::NONE;
		return;
	}

	if (scene->getScreenWidth() <= 0.0f || scene->getScreenHeight() <= 0.0f) {
		return;   // touched before the surface size is known
	}

	// The screen is split in four triangles meeting at its center: whichever
	// of the two distances is the largest picks the axis, its sign the way.
	// Touches come in pixels, so this one stays in pixels.
	float dx = event.x - scene->getScreenWidth() / 2.0f;
	float dy = event.y - scene->getScreenHeight() / 2.0f;

	if (fabsf(dx) > fabsf(dy)) {
		heldDirection = dx > 0.0f ? Direction::RIGHT : Direction::LEFT;
	} else {
		heldDirection = dy > 0.0f ? Direction::DOWN : Direction::UP;
	}
}

Direction TouchDirectionPad::getDirection() const {
	return heldDirection;
}
