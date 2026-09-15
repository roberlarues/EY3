#include "asset_loader.h"
#include <stdlib.h> // for malloc
#include "log.h"

#ifdef EY3_WITH_CV
using namespace cv;
#endif // EY3_WITH_CV
using namespace ey3;

AssetLoader::AssetLoader() {
}

AssetLoader::~AssetLoader() {
}

void AssetLoader::loadAsset(android_app* app, char* assetPath) {
}

const std::string AssetLoader::loadStringAsset(android_app* app, const char* assetPath) {
	LOGI("Loading asset %s", assetPath);

	AAssetManager* mgr = app->activity->assetManager;
	if (mgr == NULL) {
		LOGE("AssetManager not loaded");
		return NULL;
	}
	
	AAsset* asset = AAssetManager_open(mgr, assetPath, AASSET_MODE_BUFFER);
	if (asset == NULL) {
		LOGE("Asset not found");
		return NULL;
	}

	unsigned long size = AAsset_getLength64(asset);
	char* buffer = new char[size];
	AAsset_read(asset, buffer, size);
	AAsset_close(asset);

	const std::string str(buffer, size);
	free(buffer);
	return str;
}


#ifdef EY3_WITH_CV
Mat AssetLoader::loadImageAsset(android_app* app, const char* assetPath) {
	LOGI("Loading asset %s", assetPath);

	AAssetManager* mgr = app->activity->assetManager;
	if (mgr == NULL) {
		LOGE("AssetManager not loaded");
		Mat emptyMat;
		return emptyMat;
	}


	AAsset* asset = AAssetManager_open(mgr, assetPath, AASSET_MODE_BUFFER);
	if (asset == NULL) {
		LOGI("Asset not found");
		Mat emptyMat;
		return emptyMat;
	}

	unsigned long size = AAsset_getLength64(asset);
	uchar* buffer = (uchar*) malloc(sizeof(uchar) * size);
	AAsset_read(asset, buffer, size);
	AAsset_close(asset);

	std::vector<uchar> fileBytes(buffer, buffer + size);
	Mat image = imdecode(fileBytes, IMREAD_COLOR);

	LOGI("Asset loaded ok");
	return image;
}
#endif // EY3_WITH_CV
