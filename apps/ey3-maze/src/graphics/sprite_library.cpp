#include "maze/graphics/sprite_library.h"

#include "maze/graphics/sprite_sheet.h"

SpriteLibrary::SpriteLibrary() : assetLoader(nullptr), ready(false) {
}

SpriteLibrary::~SpriteLibrary() {
	std::map<std::string, SpriteSheet*>::iterator it;
	for (it = sheets.begin(); it != sheets.end(); ++it) {
		delete it->second;
	}
}

void SpriteLibrary::init(Renderer* renderer, AssetLoader* assetLoader) {
	if (ready) {
		return;
	}
	ready = true;

	this->assetLoader = assetLoader;
	this->renderer.init(assetLoader, renderer->getWidth(), renderer->getHeight());
}

Sprite SpriteLibrary::create(const char* assetPath) {
	if (!ready) {
		LOGE("SpriteLibrary::create(%s) before the GL context exists", assetPath);
		return Sprite();
	}

	std::map<std::string, SpriteSheet*>::iterator found = sheets.find(assetPath);
	if (found != sheets.end()) {
		return Sprite(found->second, &renderer);
	}

	SpriteSheet* sheet = new SpriteSheet();
	sheet->load(assetLoader, assetPath);
	sheets[assetPath] = sheet;
	return Sprite(sheet, &renderer);
}
