#include "engine.h"
#include "app_cmd.h"
#include "log.h"

using namespace ey3;

Engine::Engine(IWindow* window): window(window) {
	LOGI("Creating Engine");

	cmdHandler.addListener(&renderer);
	window->setEngine(this);

	lastFrameTime = std::chrono::steady_clock::now();
}

Engine::~Engine() {
	LOGI("Deleting Engine");
	cmdHandler.removeListener(&renderer);
}

void Engine::onCmd(int32_t cmd) {
	switch (cmd) {
        case APP_CMD_INIT_WINDOW:
			foreground = true;
            break;
        case APP_CMD_TERM_WINDOW:
			foreground = false;
            break;
		case APP_CMD_DESTROY:
			terminated = true;
            break;
    }

	cmdHandler.handleCmd(cmd, window);
}

void Engine::onInput(const InputEvent& event) {
	inputHandler.handleInput(event);
}

void Engine::pollEvents() {
	auto now = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
	lastFrameTime = now;

	window->pollEvents();
}

/*
 * GETTERS AND SETTERS
 */
IWindow* Engine::getWindow() {
	return window;
}

Renderer* Engine::getRenderer() {
	return &renderer;
}

InputHandler* Engine::getInputHandler() {
	return &inputHandler;
}

CmdHandler* Engine::getCmdHandler() {
	return &cmdHandler;
}

bool Engine::isInForeground() {
	return foreground;
}

bool Engine::hasTerminated() {
	return terminated;
}

float Engine::getDeltaTime() {
	return deltaTime;
}
