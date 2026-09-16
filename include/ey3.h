#ifndef EY3_H
#define EY3_H

#include "app_cmd.h"
#include "window.h"

#ifdef __ANDROID__
#include "window_android.h"
#else
#include "window_desktop.h"
#endif

#include "renderer.h"
#include "renderizable.h"
#include "shader.h"
#include "texture.h"
#include "asset_loader.h"
#include "cmd_handler.h"
#include "cmd_listener.h"
#include "engine.h"
#include "input_handler.h"
#include "input_listener.h"
#include "input_event.h"
#include "log.h"
#include "background.h"

#ifdef EY3_WITH_CV
#include "camera_view.h"
#ifdef __ANDROID__
#include "camera_android.h"
#else
#include "camera_desktop.h"
#endif // __ANDROID__
#endif // EY3_WITH_CV

#endif // EY3_H
