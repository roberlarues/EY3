#include "asset_loader.h"

#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#include "log.h"
#include "platform_context.h"

using namespace ey3;
#ifdef EY3_WITH_CV
using namespace cv;
#endif // EY3_WITH_CV

AssetLoader::AssetLoader() {
}

AssetLoader::~AssetLoader() {
}

const std::string AssetLoader::loadStringAsset(const char* assetPath) {
	LOGI("Loading asset %s", assetPath);

	AAssetManager* mgr = PlatformContext::getAndroidApp()->activity->assetManager;
	if (mgr == NULL) {
		LOGE("AssetManager not loaded");
		return std::string();
	}

	AAsset* asset = AAssetManager_open(mgr, assetPath, AASSET_MODE_BUFFER);
	if (asset == NULL) {
		LOGE("Asset not found");
		return std::string();
	}

	unsigned long size = AAsset_getLength64(asset);
	char* buffer = new char[size];
	AAsset_read(asset, buffer, size);
	AAsset_close(asset);

	const std::string str(buffer, size);
	delete[] buffer;
	return str;
}

#ifdef EY3_WITH_CV
Mat AssetLoader::loadImageAsset(const char* assetPath) {
	LOGI("Loading asset %s", assetPath);

	AAssetManager* mgr = PlatformContext::getAndroidApp()->activity->assetManager;
	if (mgr == NULL) {
		LOGE("AssetManager not loaded");
		return Mat();
	}

	AAsset* asset = AAssetManager_open(mgr, assetPath, AASSET_MODE_BUFFER);
	if (asset == NULL) {
		LOGI("Asset not found");
		return Mat();
	}

	unsigned long size = AAsset_getLength64(asset);
	uchar* buffer = (uchar*) malloc(sizeof(uchar) * size);
	AAsset_read(asset, buffer, size);
	AAsset_close(asset);

	std::vector<uchar> fileBytes(buffer, buffer + size);
	free(buffer);
	Mat image = imdecode(fileBytes, IMREAD_COLOR);

	LOGI("Asset loaded ok");
	return image;
}
#endif // EY3_WITH_CV
