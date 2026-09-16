#include "platform_context.h"

namespace {
	android_app* g_androidApp = nullptr;
}

void ey3::PlatformContext::setAndroidApp(android_app* app) {
	g_androidApp = app;
}

android_app* ey3::PlatformContext::getAndroidApp() {
	return g_androidApp;
}
