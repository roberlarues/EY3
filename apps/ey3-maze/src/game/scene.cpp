#include "maze/game/scene.h"

#include "maze/objects/game_object.h"

Scene::Scene(SpriteLibrary& sprites)
	: sprites(sprites), renderer(nullptr), assetLoader(nullptr),
	  screenWidth(0.0f), screenHeight(0.0f), initialized(false) {
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

	sprites.init(renderer, assetLoader);   // shared: only the first scene loads it
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

float Scene::getScreenWidth() const {
	return screenWidth;
}

float Scene::getScreenHeight() const {
	return screenHeight;
}
