#include "maze/graphics/sprite.h"

#include "maze/graphics/sprite_renderer.h"
#include "maze/graphics/sprite_sheet.h"

Sprite::Sprite()
	: sheet(nullptr), renderer(nullptr), frame(0), frameRate(0.0f), frameTimer(0.0f) {
}

Sprite::Sprite(const SpriteSheet* sheet, SpriteRenderer* renderer)
	: sheet(sheet), renderer(renderer), frame(0), frameRate(0.0f), frameTimer(0.0f) {
}

void Sprite::setFrameRate(float framesPerSecond) {
	frameRate = framesPerSecond;
	frameTimer = 0.0f;
}

void Sprite::setFrame(int frame) {
	this->frame = frame;
	frameTimer = 0.0f;
}

int Sprite::getFrame() const {
	return frame;
}

void Sprite::update(float deltaTime) {
	if (sheet == nullptr || frameRate <= 0.0f || sheet->getFrameCount() < 2) {
		return;
	}

	frameTimer += deltaTime * frameRate;
	while (frameTimer >= 1.0f) {
		frameTimer -= 1.0f;
		frame = (frame + 1) % sheet->getFrameCount();
	}
}

void Sprite::draw(const Rect& bounds, float depth) const {
	if (sheet == nullptr || renderer == nullptr) {
		return;
	}
	renderer->draw(*sheet, frame, bounds, depth);
}
