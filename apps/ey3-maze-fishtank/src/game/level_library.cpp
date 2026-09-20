#include "maze/game/level_library.h"

static const char* const LEVELS_DIR = "levels/";
static const char* const INDEX_ASSET = "levels/index.txt";

namespace {

	/** Splits text into lines, dropping the carriage return of CRLF files. */
	std::vector<std::string> splitLines(const std::string& text) {
		std::vector<std::string> lines;
		std::string line;

		for (size_t i = 0; i < text.size(); i++) {
			if (text[i] == '\n') {
				lines.push_back(line);
				line.clear();
			} else if (text[i] != '\r') {
				line += text[i];
			}
		}
		if (!line.empty()) {
			lines.push_back(line);
		}
		return lines;
	}

	/** Index entries: a file name per line, ';' comments and blanks ignored. */
	bool isEntry(const std::string& line) {
		return !line.empty() && line[0] != ';' && line[0] != ' ';
	}
}

void LevelLibrary::load(AssetLoader* assetLoader) {
	levels.clear();

	const std::vector<std::string> index = splitLines(assetLoader->loadStringAsset(INDEX_ASSET));
	if (index.empty()) {
		LOGE("No levels: %s is missing or empty", INDEX_ASSET);
		return;
	}

	for (size_t i = 0; i < index.size(); i++) {
		if (!isEntry(index[i])) {
			continue;
		}

		const std::string path = std::string(LEVELS_DIR) + index[i];
		std::vector<std::string> rows = splitLines(assetLoader->loadStringAsset(path.c_str()));

		// A trailing newline leaves an empty last row; drop those.
		while (!rows.empty() && rows.back().empty()) {
			rows.pop_back();
		}

		LevelData level(index[i], rows);
		if (level.isEmpty()) {
			LOGE("Level %s is empty, skipping it", index[i].c_str());
			continue;
		}

		LOGI("Level loaded: %s (%dx%d)", index[i].c_str(), level.getCols(), level.getRows());
		levels.push_back(level);
	}
}

int LevelLibrary::size() const {
	return (int) levels.size();
}

const LevelData& LevelLibrary::get(int index) const {
	static const LevelData none;

	if (index < 0 || index >= (int) levels.size()) {
		return none;
	}
	return levels[index];
}
