#include "renderer.h"
#include <initializer_list>
#include "app_cmd.h"
#include "log.h"
#include "window.h"

using namespace ey3;

Renderer::Renderer(): window(nullptr), enabled(false) { }

Renderer::~Renderer() {
	disable();
}

/**
 * Inits the GL context via the platform window and the initial GL state
 */
void Renderer::enable(IWindow* window) {
	if (enabled)
		return;

	LOGI("Enabling renderer");
	this->window = window;

	if (!window->createSurface()) {
		LOGW("Unable to create window surface");
		return;
	}

	if (!window->makeCurrent()) {
		LOGW("Unable to make context current");
		return;
	}

	width = window->getWidth();
	height = window->getHeight();
	LOGI("Surface size: (%d x %d)", width, height);

	// Init OpenGL ES 3
	auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION};
	for (auto name : opengl_info) {
		auto info = glGetString(name);
		LOGI("OpenGL Info: %s", info);
	}

	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Init elements
	for (int i = 0; i < renderizableElements.size(); i++) {
		renderizableElements[i]->init(this, &assetLoader);
	}

	enabled = true;
}

void Renderer::disable() {
	if (!enabled)
		return;

	LOGI("Disabling renderer");
	if (window != nullptr) {
		window->destroySurface();
	}
	enabled = false;
}

void Renderer::renderFrame() {
	if (!enabled) {
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < renderizableElements.size(); i++) {
		renderizableElements[i]->render();
	}

	window->swapBuffers();
}

void Renderer::addRenderizable(Renderizable* renderizable) {
	renderizableElements.push_back(renderizable);
	if (enabled) {
		renderizable->init(this, &assetLoader);
	}
}

void Renderer::removeRenderizable(Renderizable* renderizable) {
	int i = 0;
	while (i < renderizableElements.size()) {
		if (renderizableElements[i] == renderizable) {
			renderizableElements.erase(renderizableElements.begin() + i);
		} else {
			i++;
		}
	}
}

void Renderer::handleCmd(int32_t cmd, IWindow* window) {
	switch (cmd) {
        case APP_CMD_INIT_WINDOW:
			enable(window);
			renderFrame();
            break;
        case APP_CMD_TERM_WINDOW:
			disable();
            break;
        case APP_CMD_LOST_FOCUS:
			renderFrame();
			break;
    }
}

int32_t Renderer::getWidth() {
	return width;
}

int32_t Renderer::getHeight() {
	return height;
}
