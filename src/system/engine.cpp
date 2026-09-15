#include "engine.h"
#include <string>
#include "log.h"

using namespace ey3;

void handleCmdCallback(struct android_app* app, int32_t cmd);
int32_t handleInputCallback(struct android_app* app, AInputEvent* event);
std::string get_cmd_name(int32_t cmd);

Engine::Engine(struct android_app* app): app(app) {
	LOGI("Creating Engine");

	cmdHandler.addListener(&renderer);

	app->userData = this;
    app->onAppCmd = handleCmdCallback;
	app->onInputEvent = handleInputCallback;

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

	cmdHandler.handleCmd(cmd, app);
}

void Engine::onInput(AInputEvent* event) {
	inputHandler.handleInput(event);
}

void Engine::pollEvents() {
	auto now = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
	lastFrameTime = now;

	int events;
	struct android_poll_source* source;
	while (ALooper_pollOnce(0, nullptr, &events, (void**)&source) >= 0) {
		if (source != nullptr) {
			source->process(app, source);
		}
	}
}

/*
 * GETTERS AND SETTERS
 */
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


/*
 * CALLBACKS
 */
void handleCmdCallback(struct android_app* app, int32_t cmd) {
	LOGI("---> [ %s ]", get_cmd_name(cmd).c_str());
    Engine* engine = (Engine*) app->userData;
	engine->onCmd(cmd);
}

int32_t handleInputCallback(struct android_app* app, AInputEvent* event) {
	Engine* engine = (Engine*) app->userData;
	engine->onInput(event);
    return 0;
}

/*
 * UTILS
 */
std::string get_cmd_name(int32_t cmd) {
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
