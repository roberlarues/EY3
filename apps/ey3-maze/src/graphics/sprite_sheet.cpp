#include "maze/graphics/sprite_sheet.h"

SpriteSheet::SpriteSheet() : frameCount(1) {
}

bool SpriteSheet::load(AssetLoader* assetLoader, const char* assetPath) {
	this->assetPath = assetPath;

	cv::Mat image = assetLoader->loadImageAsset(assetPath);
	if (image.empty()) {
		LOGE("Sprite sheet not loaded: %s", assetPath);
		return false;
	}

	// A sheet is a horizontal strip of square frames.
	frameCount = image.cols / image.rows;
	if (frameCount < 1 || image.cols % image.rows != 0) {
		LOGW("%s is %dx%d, not a strip of square frames; using it as one frame",
		     assetPath, image.cols, image.rows);
		frameCount = 1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	texture.generate(image.ptr(), image.cols, image.rows,
	                 image.channels() == 4 ? GL_RGBA : GL_RGB);
	return true;
}

bool SpriteSheet::reload(AssetLoader* assetLoader) {
	invalidate();

	if (assetPath.empty()) {
		return false;   // a live source has no file behind it
	}

	// A copy: load() assigns to the member, and handing it a pointer into
	// its own buffer would be a self-assignment waiting to bite.
	const std::string path = assetPath;
	return load(assetLoader, path.c_str());
}

void SpriteSheet::invalidate() {
	texture.invalidate();
}

void SpriteSheet::bind() const {
	texture.bind();
}

int SpriteSheet::getFrameCount() const {
	return frameCount;
}
