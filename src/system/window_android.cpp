#include "window_android.h"

#include <string>

#include "engine.h"
#include "input_event.h"
#include "log.h"
#include "platform_context.h"

using namespace ey3;

static void handleCmdCallback(struct android_app* app, int32_t cmd);
static int32_t handleInputCallback(struct android_app* app, AInputEvent* event);
static std::string get_cmd_name(int32_t cmd);

WindowAndroid::WindowAndroid(android_app* app): app(app) {
	PlatformContext::setAndroidApp(app);
	app->onAppCmd = handleCmdCallback;
	app->onInputEvent = handleInputCallback;
}

void WindowAndroid::setEngine(Engine* engine) {
	this->engine = engine;
	app->userData = engine;
}

bool WindowAndroid::createSurface() {
	if (app->window == nullptr) {
		return false;
	}

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
		return false;
	}

	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, app->window, nullptr);

	const EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,  //Request opengl ES3.0 (required by GLSL "#version 300 es" shaders and glGenVertexArrays)
		EGL_NONE
	};
	context = eglCreateContext(display, config, nullptr, contextAttribs);

	if (!makeCurrent()) {
		LOGW("Unable to eglMakeCurrent");
		return false;
	}

	EGLint w, h;
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	width = w;
	height = h;

	return true;
}

void WindowAndroid::destroySurface() {
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
}

bool WindowAndroid::makeCurrent() {
	return eglMakeCurrent(display, surface, surface, context) == EGL_TRUE;
}

void WindowAndroid::swapBuffers() {
	eglSwapBuffers(display, surface);
}

void WindowAndroid::pollEvents() {
	int events;
	struct android_poll_source* source;
	while (ALooper_pollOnce(0, nullptr, &events, (void**)&source) >= 0) {
		if (source != nullptr) {
			source->process(app, source);
		}
	}
}

int32_t WindowAndroid::getWidth() {
	return width;
}

int32_t WindowAndroid::getHeight() {
	return height;
}

/*
 * CALLBACKS
 */
static void handleCmdCallback(struct android_app* app, int32_t cmd) {
	LOGI("---> [ %s ]", get_cmd_name(cmd).c_str());
    Engine* engine = (Engine*) app->userData;
	engine->onCmd(cmd);
}

static int32_t handleInputCallback(struct android_app* app, AInputEvent* event) {
	Engine* engine = (Engine*) app->userData;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION &&
			AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN) {
		int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

		InputEventType type;
		switch (action) {
			case AMOTION_EVENT_ACTION_DOWN: type = InputEventType::POINTER_DOWN; break;
			case AMOTION_EVENT_ACTION_UP: type = InputEventType::POINTER_UP; break;
			case AMOTION_EVENT_ACTION_MOVE: type = InputEventType::POINTER_MOVE; break;
			default: return 0;
		}

		InputEvent ev { type, AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0), 0 };
		engine->onInput(ev);
	}
    return 0;
}

/*
 * UTILS
 */
static std::string get_cmd_name(int32_t cmd) {
	std::string a = "";
	switch(cmd) {
		/**
		 * Command from main thread: the AInputQueue has changed.  Upon processing
		 * this command, android_app->inputQueue will be updated to the new queue
		 * (or NULL).
		 */
		case APP_CMD_INPUT_CHANGED:
			a = "INPUT CHANGED";
			break;
		/**
		 * Command from main thread: a new ANativeWindow is ready for use.  Upon
		 * receiving this command, android_app->window will contain the new window
		 * surface.
		 */
		case APP_CMD_INIT_WINDOW:
			a = "INIT WINDOW";
			break;
		/**
		 * Command from main thread: the existing ANativeWindow needs to be
		 * terminated.  Upon receiving this command, android_app->window still
		 * contains the existing window; after calling android_app_exec_cmd
		 * it will be set to NULL.
		 */
		case APP_CMD_TERM_WINDOW:
			a = "TERM WINDOW";
			break;
		/**
		 * Command from main thread: the current ANativeWindow has been resized.
		 * Please redraw with its new size.
		 */
		case APP_CMD_WINDOW_RESIZED:
			a = "WINDOW RESIZED";
			break;
		/**
		 * Command from main thread: the system needs that the current ANativeWindow
		 * be redrawn.  You should redraw the window before handing this to
		 * android_app_exec_cmd() in order to avoid transient drawing glitches.
		 */
		case APP_CMD_WINDOW_REDRAW_NEEDED:
			a = "WINDOW REDRAW NEEDED";
			break;
		/**
		 * Command from main thread: the content area of the window has changed,
		 * such as from the soft input window being shown or hidden.  You can
		 * find the new content rect in android_app::contentRect.
		 */
		case APP_CMD_CONTENT_RECT_CHANGED:
			a = "CONTENT RECT CHANGED";
			break;
		/**
		 * Command from main thread: the app's activity window has gained
		 * input focus.
		 */
		case APP_CMD_GAINED_FOCUS:
			a = "GAINED FOCUS";
			break;
		/**
		 * Command from main thread: the app's activity window has lost
		 * input focus.
		 */
		case APP_CMD_LOST_FOCUS:
			a = "LOST FOCUS";
			break;
		/**
		 * Command from main thread: the current device configuration has changed.
		 */
		case APP_CMD_CONFIG_CHANGED:
			a = "CONFIG CHANGED";
			break;
		/**
		 * Command from main thread: the system is running low on memory.
		 * Try to reduce your memory use.
		 */
		case APP_CMD_LOW_MEMORY:
			a = "LOW MEMORY";
			break;
		/**
		 * Command from main thread: the app's activity has been started.
		 */
		case APP_CMD_START:
			a = "START";
			break;
		/**
		 * Command from main thread: the app's activity has been resumed.
		 */
		case APP_CMD_RESUME:
			a = "RESUME";
			break;
		/**
		 * Command from main thread: the app should generate a new saved state
		 * for itself, to restore from later if needed.  If you have saved state,
		 * allocate it with malloc and place it in android_app.savedState with
		 * the size in android_app.savedStateSize.  The will be freed for you
		 * later.
		 */
		case APP_CMD_SAVE_STATE:
			a = "SAVE STATE";
			break;
		/**
		 * Command from main thread: the app's activity has been paused.
		 */
		case APP_CMD_PAUSE:
			a = "PAUSE";
			break;
		/**
		 * Command from main thread: the app's activity has been stopped.
		 */
		case APP_CMD_STOP:
			a = "STOP";
			break;
		/**
		 * Command from main thread: the app's activity is being destroyed,
		 * and waiting for the app thread to clean up and exit before proceeding.
		 */
		case APP_CMD_DESTROY:
			a = "DESTROY";
			break;
		default:
			a = "UNKNOW";
	}
	return a;
}
