#include <jni.h>

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

	// Init EGL. The depth buffer is not optional: without it glEnable(
	// GL_DEPTH_TEST) silently does nothing and whatever is drawn last wins,
	// which in 3D means seeing the far side of everything. 24 bits first,
	// 16 as a fallback for devices that offer nothing better.
	EGLint attribs[] = {
		EGL_SURFACE_TYPE,
		EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
	    LOGI("eglGetDisplay() returned error %d", eglGetError());
	}

	if (!eglInitialize(display, 0, 0)) {
		LOGI("eglInitialize() returned error %d", eglGetError());
	}

	EGLConfig config = nullptr;
	EGLint numConfigs;
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	if (numConfigs == 0) {
		LOGW("No EGLConfig with a 24 bit depth buffer; asking for 16");
		attribs[9] = 16;   // the value that follows EGL_DEPTH_SIZE
		eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	}
	LOGI("Num eglConfigs: %d", numConfigs);
	if (numConfigs == 0) {
		LOGI("eglChooseConfig() returned error %d", eglGetError());
	}

	EGLint depthBits = 0;
	eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depthBits);
	LOGI("EGL depth buffer: %d bits", depthBits);

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

namespace {
	// Millimetres per pixel from the screen density the activity was
	// configured with, or 0 when Android reports no usable value.
	/**
	 * The density bucket Android uses to pick resources (160, 240, 320, 420,
	 * 480...). Always available, but quantised: on a panel of 401 real dpi it
	 * says 480, which is a fifth off.
	 */
	float densityBucketDpi(android_app* app) {
		if (app == nullptr || app->config == nullptr) {
			return 0.0f;
		}

		int32_t density = AConfiguration_getDensity(app->config);
		if (density == ACONFIGURATION_DENSITY_DEFAULT || density == ACONFIGURATION_DENSITY_NONE
			|| density == ACONFIGURATION_DENSITY_ANY || density <= 0) {
			return 0.0f;
		}
		return (float) density;
	}

	/**
	 * The panel's real dots per inch, from DisplayMetrics.
	 *
	 * Reached by calling the framework through JNI -- the activity and the VM
	 * come with ANativeActivity, so this needs no Java of our own and the
	 * manifest stays android:hasCode="false". android_main runs on its own
	 * thread, hence the attach.
	 *
	 * Returns false if anything goes wrong, and the caller falls back to the
	 * bucket. That also covers the manufacturers who fill these fields with
	 * nonsense, which the caller checks for.
	 */
	bool displayMetricsDpi(android_app* app, float& xDpi, float& yDpi) {
		if (app == nullptr || app->activity == nullptr || app->activity->vm == nullptr) {
			return false;
		}

		// Attach only if this thread is not attached already: detaching one
		// that somebody else attached would pull the rug from under them.
		JavaVM* vm = app->activity->vm;
		JNIEnv* env = nullptr;
		bool attached = false;
		if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
			if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK || env == nullptr) {
				return false;
			}
			attached = true;
		}

		bool ok = false;
		jclass activityClass = env->GetObjectClass(app->activity->clazz);
		jobject resources = nullptr;
		jclass resourcesClass = nullptr;
		jobject metrics = nullptr;
		jclass metricsClass = nullptr;

		jmethodID getResources = env->GetMethodID(activityClass, "getResources",
			"()Landroid/content/res/Resources;");
		if (getResources != nullptr) {
			resources = env->CallObjectMethod(app->activity->clazz, getResources);
		}
		if (resources != nullptr) {
			resourcesClass = env->GetObjectClass(resources);
			jmethodID getMetrics = env->GetMethodID(resourcesClass, "getDisplayMetrics",
				"()Landroid/util/DisplayMetrics;");
			if (getMetrics != nullptr) {
				metrics = env->CallObjectMethod(resources, getMetrics);
			}
		}
		if (metrics != nullptr) {
			metricsClass = env->GetObjectClass(metrics);
			jfieldID xField = env->GetFieldID(metricsClass, "xdpi", "F");
			jfieldID yField = env->GetFieldID(metricsClass, "ydpi", "F");
			if (xField != nullptr && yField != nullptr) {
				xDpi = env->GetFloatField(metrics, xField);
				yDpi = env->GetFloatField(metrics, yField);
				ok = true;
			}
		}

		// A failed lookup leaves an exception pending, and the next JNI call
		// on this thread would abort the process if we left it there.
		if (env->ExceptionCheck()) {
			env->ExceptionClear();
			ok = false;
		}

		// Locals are only freed automatically on returning to Java, which is
		// not where this thread is going.
		if (metricsClass != nullptr) env->DeleteLocalRef(metricsClass);
		if (metrics != nullptr) env->DeleteLocalRef(metrics);
		if (resourcesClass != nullptr) env->DeleteLocalRef(resourcesClass);
		if (resources != nullptr) env->DeleteLocalRef(resources);
		if (activityClass != nullptr) env->DeleteLocalRef(activityClass);

		if (attached) {
			vm->DetachCurrentThread();
		}
		return ok;
	}

	/**
	 * Millimetres per pixel, from the real dots per inch when the device
	 * reports something believable and from the density bucket otherwise.
	 * Worked out once: neither value changes while the app runs.
	 */
	void resolveMillimetresPerPixel(android_app* app, float& xMm, float& yMm) {
		float bucket = densityBucketDpi(app);
		float xDpi = 0.0f, yDpi = 0.0f;

		if (displayMetricsDpi(app, xDpi, yDpi) && xDpi > 0.0f && yDpi > 0.0f) {
			// Believable means "in the same world as the bucket". Some
			// devices report 160 flat, or four digits; those get dropped.
			bool sane = bucket <= 0.0f
				|| (xDpi > bucket * 0.5f && xDpi < bucket * 2.0f
					&& yDpi > bucket * 0.5f && yDpi < bucket * 2.0f);
			if (sane) {
				LOGI("Screen density: %.0fx%.0f real dpi (bucket says %.0f)", xDpi, yDpi, bucket);
				xMm = 25.4f / xDpi;
				yMm = 25.4f / yDpi;
				return;
			}
			LOGW("Screen density: ignoring DisplayMetrics (%.0fx%.0f dpi, bucket %.0f)",
			     xDpi, yDpi, bucket);
		}

		if (bucket <= 0.0f) {
			LOGW("Screen density: the device reports none; physical size unknown");
			xMm = yMm = 0.0f;
			return;
		}
		LOGI("Screen density: %.0f dpi from the density bucket", bucket);
		xMm = yMm = 25.4f / bucket;
	}
}

float WindowAndroid::getPhysicalWidthMm() {
	if (millimetresPerPixelX < 0.0f) {
		resolveMillimetresPerPixel(app, millimetresPerPixelX, millimetresPerPixelY);
	}
	return width * millimetresPerPixelX;
}

float WindowAndroid::getPhysicalHeightMm() {
	if (millimetresPerPixelY < 0.0f) {
		resolveMillimetresPerPixel(app, millimetresPerPixelX, millimetresPerPixelY);
	}
	return height * millimetresPerPixelY;
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
