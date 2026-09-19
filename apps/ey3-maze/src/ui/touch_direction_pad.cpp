#include "maze/ui/touch_direction_pad.h"

#include <math.h>

#include "maze/game/depths.h"
#include "maze/game/scene.h"

// Size of the arrows, how much bigger the held one is drawn, and how far they
// sit from the screen edge. Being interface, they are measured against the
// screen's shorter side, not against anything in the world.
static const float ARROW_SIZE = 0.08f;
static const float ARROW_HELD_SCALE = 1.5f;
static const float ARROW_MARGIN = 0.03f;

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
	float screenWidth = scene->getScreenWidth();
	float screenHeight = scene->getScreenHeight();
	float unit = fminf(screenWidth, screenHeight);
	float margin = unit * ARROW_MARGIN;

	for (int i = 0; i < 4; i++) {
		Direction direction = ARROW_DIRECTIONS[i];
		float size = unit * ARROW_SIZE;
		if (direction == heldDirection) {
			size *= ARROW_HELD_SCALE;
		}

		// Centered against the middle of one screen edge.
		Rect bounds = {(screenWidth - size) / 2.0f, (screenHeight - size) / 2.0f, size, size};
		if (direction == Direction::UP) {
			bounds.y = margin;
		} else if (direction == Direction::DOWN) {
			bounds.y = screenHeight - size - margin;
		} else if (direction == Direction::LEFT) {
			bounds.x = margin;
		} else {
			bounds.x = screenWidth - size - margin;
		}

		arrows[i].draw(bounds, DEPTH_OVERLAY);
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
