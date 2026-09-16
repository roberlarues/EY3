#ifndef EY3ASSET_LOADER_H
#define EY3ASSET_LOADER_H

#include <string>

#ifdef EY3_WITH_CV
#include <opencv2/opencv.hpp>
#endif // EY3_WITH_CV

namespace ey3 {

	/**
	 * Allows to load assets from the 'assets' folder of the running app.
	 */
	class AssetLoader {
		public:
			AssetLoader();
			virtual ~AssetLoader();
			const std::string loadStringAsset(const char* assetPath);
#ifdef EY3_WITH_CV
			cv::Mat loadImageAsset(const char* assetPath);
#endif // EY3_WITH_CV
	};
}

#endif // EY3ASSET_LOADER_H
