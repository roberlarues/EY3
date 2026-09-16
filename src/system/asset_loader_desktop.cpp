#include "asset_loader.h"

#include <climits>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "log.h"

using namespace ey3;
#ifdef EY3_WITH_CV
using namespace cv;
#endif // EY3_WITH_CV

namespace {

	// Resolves assets relative to the running executable (not the cwd), so
	// the binary can be launched from anywhere as long as an 'assets' folder
	// sits next to it (see desktop/CMakeLists.txt, which symlinks it there).
	std::string executableDir() {
		char buffer[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
		if (len <= 0) {
			return ".";
		}
		buffer[len] = '\0';

		std::string path(buffer);
		size_t lastSlash = path.find_last_of('/');
		return (lastSlash == std::string::npos) ? "." : path.substr(0, lastSlash);
	}

	std::string assetPathFor(const char* assetPath) {
		return executableDir() + "/assets/" + assetPath;
	}
}

AssetLoader::AssetLoader() {
}

AssetLoader::~AssetLoader() {
}

const std::string AssetLoader::loadStringAsset(const char* assetPath) {
	std::string fullPath = assetPathFor(assetPath);
	LOGI("Loading asset %s", fullPath.c_str());

	std::ifstream file(fullPath);
	if (!file) {
		LOGE("Asset not found: %s", fullPath.c_str());
		return std::string();
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

#ifdef EY3_WITH_CV
Mat AssetLoader::loadImageAsset(const char* assetPath) {
	std::string fullPath = assetPathFor(assetPath);
	LOGI("Loading asset %s", fullPath.c_str());

	Mat image = imread(fullPath, IMREAD_COLOR);
	if (image.empty()) {
		LOGE("Asset not found: %s", fullPath.c_str());
	}
	return image;
}
#endif // EY3_WITH_CV
