#ifndef EY3ASSET_LOADER_H
#define EY3ASSET_LOADER_H

#include <string>

#include <opencv2/opencv.hpp>

namespace ey3 {

	/**
	 * Allows to load assets from the 'assets' folder of the running app.
	 */
	class AssetLoader {
		public:
			AssetLoader();
			virtual ~AssetLoader();
			const std::string loadStringAsset(const char* assetPath);
			cv::Mat loadImageAsset(const char* assetPath);
	};
}

#endif // EY3ASSET_LOADER_H
