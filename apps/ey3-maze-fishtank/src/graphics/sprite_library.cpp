#include "maze/graphics/sprite_library.h"

#include "maze/graphics/sprite_sheet.h"

SpriteLibrary::SpriteLibrary() : assetLoader(nullptr), ready(false), contextGeneration(0) {
}

SpriteLibrary::~SpriteLibrary() {
	std::map<std::string, SpriteSheet*>::iterator it;
	for (it = sheets.begin(); it != sheets.end(); ++it) {
		delete it->second;
	}
}

void SpriteLibrary::init(Renderer* renderer, AssetLoader* assetLoader) {
	this->assetLoader = assetLoader;
	ready = true;

	int32_t generation = renderer->getContextGeneration();
	if (generation == contextGeneration) {
		return;   // same context as last time: the textures are still alive
	}
	contextGeneration = generation;

	// A new context: everything these sheets were holding is gone.
	int reloaded = 0;
	std::map<std::string, SpriteSheet*>::iterator it;
	for (it = sheets.begin(); it != sheets.end(); ++it) {
		if (it->second->reload(assetLoader)) {
			reloaded++;
		}
	}
	if (reloaded > 0) {
		LOGI("Sprite sheets uploaded again onto the new GL context: %d", reloaded);
	}
}

Sprite SpriteLibrary::create(const char* assetPath) {
	if (!ready) {
		LOGE("SpriteLibrary::create(%s) before the GL context exists", assetPath);
		return Sprite();
	}

	std::map<std::string, SpriteSheet*>::iterator found = sheets.find(assetPath);
	if (found != sheets.end()) {
		return Sprite(found->second);
	}

	SpriteSheet* sheet = new SpriteSheet();
	sheet->load(assetLoader, assetPath);
	sheets[assetPath] = sheet;
	return Sprite(sheet);
}
