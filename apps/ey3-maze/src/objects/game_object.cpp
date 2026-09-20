#include "maze/objects/game_object.h"

#include "maze/game/scene.h"

GameObject::GameObject(Scene* scene) : scene(scene) {
}

GameObject::~GameObject() {
}

void GameObject::init(Renderer* renderer, AssetLoader* assetLoader) {
	(void) renderer;
	(void) assetLoader;
	onInit();
}

void GameObject::onInit() {
}

void GameObject::update(float deltaTime) {
	(void) deltaTime;
}

bool GameObject::blocks(int col, int row) const {
	(void) col;
	(void) row;
	return false;
}

void GameObject::handleInput(const InputEvent& event) {
	(void) event;
}
