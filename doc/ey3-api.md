# ey3 API reference

What's available once you `#include <ey3.h>` (everything lives in namespace
`ey3`), and how the pieces fit together. This is the reference to reach for
instead of grepping headers -- useful in `templates/ey3-starter` in
particular, since it doesn't vendor a copy of them (see its README.md).

## Core loop: Engine, IWindow, Renderer

```cpp
IWindow* window = ...;       // WindowDesktop or WindowAndroid
Engine engine(window);
engine.run();                 // pumps events + renders until terminated
```

- **`IWindow`** -- abstracts the platform window/surface/GL-context/event
  loop. You don't call most of its methods directly; you construct one of
  its two implementations and hand it to `Engine`:
  - `WindowDesktop(const char* title, int32_t width, int32_t height)` (GLFW).
    `MOBILE_WIDTH_PORTRAIT`/`MOBILE_HEIGHT_PORTRAIT` (and the `_LANDSCAPE`
    pair) in `<system/window_desktop.h>` give an approximate mobile-in-hand
    size for desktop testing.
  - `WindowAndroid(android_app* app)` -- construct from `android_main`'s
    `android_app*`.
- **`Engine`** -- owns the `Renderer`, `InputHandler` and `CmdHandler`, and
  drives the loop.
  - `Engine(IWindow* window)`
  - `void run()` -- the standard loop: while in foreground, `pollEvents()`
    then `renderFrame()`, until `onCmd(APP_CMD_DESTROY)`. Apps with extra
    per-frame work (e.g. polling a camera every tick) can call
    `pollEvents()`/`getRenderer()->renderFrame()` directly in their own loop
    instead of calling `run()`.
  - `void onCmd(int32_t cmd)` -- feed a lifecycle command in (desktop's
    `main_desktop.cpp` calls `onCmd(APP_CMD_INIT_WINDOW)` once up front since
    GLFW creates the window synchronously; Android wires this up for you via
    `WindowAndroid`, so `android_main` never needs to call it).
  - `void onInput(const InputEvent& event)` -- fed by `IWindow` on
    click/touch; you don't normally call this yourself.
  - `Renderer* getRenderer()`, `InputHandler* getInputHandler()`,
    `CmdHandler* getCmdHandler()`, `IWindow* getWindow()`.
  - `bool isInForeground()`, `bool hasTerminated()`, `float getDeltaTime()`
    (seconds since the last frame).
- **`Renderer`** -- clears the screen and calls `render()` on every
  registered `Renderizable`, every frame.
  - `void addRenderizable(Renderizable*)` / `removeRenderizable(Renderizable*)`
  - `int32_t getWidth()`, `int32_t getHeight()` -- current surface size.

## Writing your own scene: Renderizable

```cpp
class MyShape : public Renderizable {
public:
    void init(Renderer* renderer, AssetLoader* assetLoader) override {
        shader.init(assetLoader, "shaders/my.vs", "shaders/my.frag");
        // ...glGenVertexArrays/glGenBuffers/... as usual GLES3 setup
    }
    void render() override {
        glUseProgram(shader.getProgram());
        // ...glBindVertexArray/glDrawArrays/...
    }
private:
    Shader shader;
};

MyShape shape;
engine.getRenderer()->addRenderizable(&shape);
```

- **`Renderizable`** -- pure interface, `init(Renderer*, AssetLoader*)` +
  `render()`. `init` is called once (immediately if the renderer is already
  enabled, otherwise when it becomes enabled); `render()` every frame.
  `render()`/`init()` run with the GL context current, so plain
  `GLES3/gl3.h` calls (any of the `#version 300 es` API) work directly.
- See `apps/ey3-triangle` (main EY3 repo) for a complete, minimal working
  example, including a shader pair under `assets/shaders/`.

## Input: InputListener, InputEvent

```cpp
class MyShape : public Renderizable, public InputListener {
    void handleInput(const InputEvent& event) override {
        if (event.type == InputEventType::POINTER_DOWN) { /* event.x, event.y */ }
    }
};
engine.getInputHandler()->addListener(&shape);
```

- `InputEvent { InputEventType type; float x, y; int32_t pointerId; }`.
- `InputEventType`: `POINTER_DOWN`, `POINTER_MOVE`, `POINTER_UP` -- desktop
  mouse and Android touch both map to these, uniformly.
- `InputHandler::addListener`/`removeListener(InputListener*)`.

## Lifecycle: CmdListener, app_cmd

```cpp
class MyThing : public CmdListener {
    void handleCmd(int32_t cmd, IWindow* window) override {
        if (cmd == APP_CMD_INIT_WINDOW) { /* GL context just became current */ }
    }
};
engine.getCmdHandler()->addListener(&myThing);
```

- Command codes (`<system/app_cmd.h>`): on Android these are literally
  `android_native_app_glue`'s `APP_CMD_*` values; on desktop, EY3-owned
  values with the same names/meaning for the ones that apply
  (`APP_CMD_INIT_WINDOW`, `APP_CMD_TERM_WINDOW`, `APP_CMD_GAINED_FOCUS`,
  `APP_CMD_LOST_FOCUS`, `APP_CMD_DESTROY`, ...). Only bother with this if you
  need to react to window/focus lifecycle directly -- most apps don't.

## Assets: AssetLoader

- `const std::string loadStringAsset(const char* assetPath)` -- reads a text
  file from the app's `assets/` folder (desktop: relative to the executable,
  via the `assets` symlink set up by `ey3_add_desktop_app` in
  `desktop/CMakeLists.txt`; Android: via `AAssetManager`). `Shader::init`
  uses this for shader source.
- `cv::Mat loadImageAsset(const char* assetPath)` -- only if built with
  `EY3_WITH_CV` (see below).

## Graphics helpers: Shader, Texture, Background

- **`Shader`**
  - `void init(AssetLoader*, const char* vShaderAssetPath, const char* fShaderAssetPath)`
    -- compiles+links a `#version 300 es` vertex/fragment pair loaded from
    `assets/`.
  - `void init()` -- built-in default pass-through shader, no assets needed
    (this is how `ey3-orb-demo` gets away with no `assets/` folder at all).
  - `GLuint getProgram()`.
- **`Texture`** -- `generate(GLubyte* data, GLuint width, GLuint height)`,
  `bind()`, `update(GLubyte* data)`.
- **`Background`** -- a ready-made `Renderizable` that fills the screen with
  a texture: `Background(GLubyte* pixels, GLuint width, GLuint height)`.

## Camera / computer vision (only with `EY3_WITH_CV`)

Requires OpenCV (see each platform's build notes) and pulls `cv::Mat`/
`cv::VideoCapture` into your own code -- not hidden behind an opaque API, see
`templates/ey3-starter/README.md` for what that means for a project using
this.

- **`Camera`** (`<system/camera_desktop.h>`/`camera_android.h`, same API on
  both):
  - `Camera(int32_t deviceIndex = 0)`
  - `bool open(CameraFacing facing, int32_t width, int32_t height)`,
    `void close()`
  - `bool getFrame(cv::OutputArray out)`
  - `bool isLoaded()`, `bool isOpened()`, `int32_t getFrameWidth()/getFrameHeight()`,
    `int32_t getImageRotation()`
  - `bool hasPermission()`, `void requestPermission()` -- real runtime
    permission dance on Android; no-ops on desktop (no such concept for a
    webcam), kept so app code is identical on both platforms.
  - It's a `CmdListener` -- register it on `engine.getCmdHandler()` so it
    opens/closes itself with the window lifecycle; see
    `apps/ey3-orb-demo/src/program.cpp` in the main repo.
- **`CameraView`** -- a `Renderizable` that displays live camera frames as a
  full-screen background: `CameraView(int32_t frameWidth, int32_t frameHeight)`,
  `void setFrame(cv::Mat* frame)`.

## Logging

`<system/log.h>`: `LOGI(fmt, ...)`, `LOGW(fmt, ...)`, `LOGE(fmt, ...)` --
`__android_log_print` on Android, `fprintf(stdout/stderr, ...)` on desktop.
Define `LOGNAME` (a build-system `-D`, already done for you by
`find_package(ey3)`/the starter's `android/CMakeLists.txt`) to tag your own
app's log lines.
