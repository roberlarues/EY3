#ifndef EY3ASSET_LOADER_H
#define EY3ASSET_LOADER_H

#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <string>

#ifdef EY3_WITH_CV
#include <opencv2/opencv.hpp>
#endif // EY3_WITH_CV

namespace ey3 {

	/**
	 * Allows to load assets from the 'assets' folder.
	 */
	class AssetLoader {
		public:
			AssetLoader();
			virtual ~AssetLoader();
			void loadAsset(android_app* app, char* assetPath);
			const std::string loadStringAsset(android_app* app, const char* assetPath);
#ifdef EY3_WITH_CV
			cv::Mat loadImageAsset(android_app* app, const char* assetPath);
#endif // EY3_WITH_CV
	};
}

#endif // EY3RENDERER_H 
