#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "asset_loader.h"
#include "log.h"

using namespace ey3;

/**
 * Decoding images is the same job on every platform once the bytes are in
 * memory, so it lives here instead of being written twice in the platform
 * loaders -- and, because it is a translation unit of its own, an app that
 * never loads an image does not drag OpenCV's decoders into its binary.
 */
cv::Mat AssetLoader::loadImageAsset(const char* assetPath) {
	const std::string bytes = loadStringAsset(assetPath);
	if (bytes.empty()) {
		LOGE("Image asset missing or empty: %s", assetPath);
		return cv::Mat();
	}

	const std::vector<uchar> buffer(bytes.begin(), bytes.end());
	cv::Mat image = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
	if (image.empty()) {
		LOGE("Image asset could not be decoded: %s", assetPath);
		return image;
	}

	// Transparency is kept: an image with an alpha channel comes back with
	// four channels, and the caller passes GL_RGBA to Texture::generate.
	// Either way the order is RGB(A), the same as Camera::getFrame and the
	// one GL wants, so nothing else has to remember which way round it is.
	if (image.channels() == 4) {
		cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
	} else if (image.channels() == 3) {
		cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
	} else if (image.channels() == 1) {
		cv::cvtColor(image, image, cv::COLOR_GRAY2RGB);
	} else {
		LOGE("Image asset %s has %d channels, which is not supported",
		     assetPath, image.channels());
		return cv::Mat();
	}
	return image;
}
