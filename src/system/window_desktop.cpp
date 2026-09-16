#include "window_desktop.h"

#include <GLFW/glfw3.h>

#include "app_cmd.h"
#include "engine.h"
#include "input_event.h"
#include "log.h"

using namespace ey3;

WindowDesktop::WindowDesktop(const char* title, int32_t width, int32_t height)
	: width(width), height(height) {

	if (!glfwInit()) {
		LOGE("glfwInit failed");
		return;
	}

	// EGL + GLES3 context, so the shared GLES3 renderer/shaders don't need a
	// desktop-GL dialect: Mesa implements GLES natively via EGL on Linux.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!glfwWindow) {
		LOGE("glfwCreateWindow failed");
		return;
	}

	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
	glfwSetCursorPosCallback(glfwWindow, cursorPosCallback);
	glfwSetFramebufferSizeCallback(glfwWindow, framebufferSizeCallback);
}

WindowDesktop::~WindowDesktop() {
	destroySurface();
	if (glfwWindow != nullptr) {
		glfwDestroyWindow(glfwWindow);
	}
	glfwTerminate();
}

void WindowDesktop::setEngine(Engine* engine) {
	this->engine = engine;
}

bool WindowDesktop::createSurface() {
	if (glfwWindow == nullptr) {
		return false;
	}

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(glfwWindow, &fbWidth, &fbHeight);
	width = fbWidth;
	height = fbHeight;

	return makeCurrent();
}

void WindowDesktop::destroySurface() {
	// The GL context lives as long as the GLFWwindow itself; nothing to tear
	// down separately here (unlike Android, where the surface can outlive or
	// be revoked independently of the app process).
}

bool WindowDesktop::makeCurrent() {
	if (glfwWindow == nullptr) {
		return false;
	}
	glfwMakeContextCurrent(glfwWindow);
	return true;
}

void WindowDesktop::swapBuffers() {
	glfwSwapBuffers(glfwWindow);
}

void WindowDesktop::pollEvents() {
	if (glfwWindow == nullptr) {
		return;
	}

	if (glfwWindowShouldClose(glfwWindow)) {
		engine->onCmd(APP_CMD_TERM_WINDOW);
		engine->onCmd(APP_CMD_DESTROY);
		return;
	}

	glfwPollEvents();
}

int32_t WindowDesktop::getWidth() {
	return width;
}

int32_t WindowDesktop::getHeight() {
	return height;
}

/*
 * CALLBACKS
 */
void WindowDesktop::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT) {
		return;
	}

	auto* self = static_cast<WindowDesktop*>(glfwGetWindowUserPointer(window));
	if (self == nullptr || self->engine == nullptr) {
		return;
	}

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	InputEvent event {
		(action == GLFW_PRESS) ? InputEventType::POINTER_DOWN : InputEventType::POINTER_UP,
		(float) x, (float) y, 0
	};
	self->engine->onInput(event);
}

void WindowDesktop::cursorPosCallback(GLFWwindow* window, double x, double y) {
	auto* self = static_cast<WindowDesktop*>(glfwGetWindowUserPointer(window));
	if (self == nullptr || self->engine == nullptr) {
		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		return;
	}

	InputEvent event { InputEventType::POINTER_MOVE, (float) x, (float) y, 0 };
	self->engine->onInput(event);
}

void WindowDesktop::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	auto* self = static_cast<WindowDesktop*>(glfwGetWindowUserPointer(window));
	if (self == nullptr) {
		return;
	}
	self->width = width;
	self->height = height;
}
