#include "maze/game/scene.h"

#include "maze/objects/game_object.h"

// What to assume when the platform will not say how big the screen is.
static const float FALLBACK_DPI = 96.0f;
static const float MM_PER_INCH = 25.4f;

Scene::Scene(SpriteLibrary& sprites, MeshRenderer& meshes)
	: sprites(sprites), meshes(meshes), renderer(nullptr), assetLoader(nullptr),
	  screenWidth(0.0f), screenHeight(0.0f), screenWidthCm(0.0f), screenHeightCm(0.0f),
	  initialized(false) {
}

Scene::~Scene() {
	for (size_t i = 0; i < objects.size(); i++) {
		delete objects[i];
	}
}

const std::vector<GameObject*>& Scene::getObjects() const {
	return objects;
}

void Scene::onInit() {
}

void Scene::add(GameObject* object) {
	objects.push_back(object);

	if (initialized) {
		object->init(renderer, assetLoader);
	}
}

void Scene::update(float deltaTime) {
	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->update(deltaTime);
	}
}

void Scene::init(Renderer* renderer, AssetLoader* assetLoader) {
	this->renderer = renderer;
	this->assetLoader = assetLoader;
	screenWidth = (float) renderer->getWidth();
	screenHeight = (float) renderer->getHeight();

	IWindow* window = renderer->getWindow();
	screenWidthCm = window != nullptr ? window->getPhysicalWidthMm() / 10.0f : 0.0f;
	screenHeightCm = window != nullptr ? window->getPhysicalHeightMm() / 10.0f : 0.0f;
	if (screenWidthCm <= 0.0f || screenHeightCm <= 0.0f) {
		screenWidthCm = screenWidth / FALLBACK_DPI * MM_PER_INCH / 10.0f;
		screenHeightCm = screenHeight / FALLBACK_DPI * MM_PER_INCH / 10.0f;
		LOGW("The platform did not report a screen size; assuming %.0f dpi", FALLBACK_DPI);
	}
	LOGI("Surface: %.0fx%.0f px, %.1fx%.1f cm",
	     screenWidth, screenHeight, screenWidthCm, screenHeightCm);

	// Shared with every other scene: only the first one does the work.
	sprites.init(renderer, assetLoader);
	meshes.init(renderer, assetLoader);
	onInit();

	initialized = true;
	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->init(renderer, assetLoader);
	}
}

void Scene::render() {
	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->render();
	}
}

void Scene::handleInput(const InputEvent& event) {
	for (size_t i = 0; i < objects.size(); i++) {
		objects[i]->handleInput(event);
	}
}

SpriteLibrary& Scene::getSprites() {
	return sprites;
}

MeshRenderer& Scene::getMeshes() {
	return meshes;
}

float Scene::getScreenWidth() const {
	return screenWidth;
}

float Scene::getScreenHeight() const {
	return screenHeight;
}

float Scene::getScreenWidthCm() const {
	return screenWidthCm;
}

float Scene::getScreenHeightCm() const {
	return screenHeightCm;
}
