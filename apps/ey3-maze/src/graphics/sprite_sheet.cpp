#include "maze/graphics/sprite_sheet.h"

#include <opencv2/imgproc.hpp>

SpriteSheet::SpriteSheet() : frameCount(1) {
}

bool SpriteSheet::load(AssetLoader* assetLoader, const char* assetPath) {
	cv::Mat image = assetLoader->loadImageAsset(assetPath);
	if (image.empty()) {
		LOGE("Sprite sheet not loaded: %s", assetPath);
		return false;
	}

	// loadImageAsset hands over BGR, which is OpenCV's order, not GL's.
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

	// A sheet is a horizontal strip of square frames.
	frameCount = image.cols / image.rows;
	if (frameCount < 1 || image.cols % image.rows != 0) {
		LOGW("%s is %dx%d, not a strip of square frames; using it as one frame",
		     assetPath, image.cols, image.rows);
		frameCount = 1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	texture.generate(image.ptr(), image.cols, image.rows);
	return true;
}

void SpriteSheet::bind() const {
	texture.bind();
}

int SpriteSheet::getFrameCount() const {
	return frameCount;
}
