#include "renderer.h"
#include <initializer_list>
#include "log.h"

using namespace ey3;

Renderer::Renderer(): enabled(false) { }

Renderer::~Renderer() {
	disable();
}

/**
 * Inits OpenGL and EGL
 */
void Renderer::enable(android_app* app) {
	if (enabled) 
		return;

	LOGI("Enabling renderer");
	this->app = app;
	
	// Init EGL
	EGLint attribs[] = {
		EGL_SURFACE_TYPE,
		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
	    LOGI("eglGetDisplay() returned error %d", eglGetError());
	}

	if (!eglInitialize(display, 0, 0)) {
		LOGI("eglInitialize() returned error %d", eglGetError());
	}


	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	LOGI("Num eglConfigs: %d", numConfigs);
	if (numConfigs == 0) {
		LOGI("eglChooseConfig() returned error %d", eglGetError());
	}

	if (config == nullptr) {
		LOGW("Unable to initialize EGLConfig");
		return;
	}

	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, app->window, nullptr);


	const EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,  //Request opengl ES2.0
		EGL_NONE
	};
	context = eglCreateContext(display, config, nullptr, contextAttribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return;
	}

	EGLint w, h;
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	width = w;
	height = h;
	LOGI("Surface size: (%d x %d)", width, height);

	// Init OpenGL ES 3
	auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION};
	for (auto name : opengl_info) {
		auto info = glGetString(name);
		LOGI("OpenGL Info: %s", info);
	}

	glViewport(0, 0, w, h);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Init elements
	for (int i = 0; i < renderizableElements.size(); i++) {
		renderizableElements[i]->init(app);
	}

	enabled = true;
}

void Renderer::disable() {
	if (!enabled) 
		return;

	LOGI("Disabling renderer");
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
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

	eglSwapBuffers(display, surface);
}

void Renderer::addRenderizable(Renderizable* renderizable) {
	renderizableElements.push_back(renderizable);
	if (enabled) {
		renderizable->init(app);
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

void Renderer::handleCmd(int32_t cmd, android_app* app) {
	switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL) {
				enable(app);
				renderFrame();
            }
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
