# ey3 API reference

What's available once you `#include <ey3.h>` (everything lives in namespace
`ey3`), and how the pieces fit together. This is the reference to reach for
instead of grepping headers -- useful in `templates/ey3-starter` in
particular, since it doesn't vendor a copy of them (see its README.md).

## Core loop: Engine, IWindow, Renderer

```cpp
IWindow* window = ...;       // WindowDesktop or WindowAndroid
Engine engine(window);

// The loop belongs to the app: every app writes this (or its own variant --
// advancing a game by getDeltaTime(), polling a camera, ...).
while (!engine.hasTerminated()) {
    engine.pollEvents();
    if (engine.isInForeground()) {
        engine.getRenderer()->renderFrame();
    }
}
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
  - `float getPhysicalWidthMm()`, `float getPhysicalHeightMm()` -- the size of
    the surface in the real world, or 0 when the platform cannot tell.
    Anything drawn in real units needs it -- see `HeadTracker` below.
    Android asks the framework for the panel's real dots per inch
    (`DisplayMetrics.xdpi`, reached through JNI, no Java of your own needed)
    and falls back to the density bucket when the device reports something
    unbelievable, which some do; desktop derives it from the monitor's EDID.
    Treat it as good to a few percent, not as a calliper.
- **`Engine`** -- owns the `Renderer`, `InputHandler` and `CmdHandler`, and
  wires them to the platform window. The loop itself is the app's (see
  above).
  - `Engine(IWindow* window)`
  - `void pollEvents()` -- pumps the platform's event queue (which is what
    delivers input and lifecycle commands) and measures the frame time
    returned by `getDeltaTime()`. Call it once per iteration of your loop,
    before rendering. The engine deliberately has no `run()` of its own: the
    loop above is three lines, and every app beyond the simplest one wants
    its own work in it.
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
  - `IWindow* getWindow()` -- the window being drawn on, for the few things
    that need more of it than its pixel size.

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
  `render()`. `init` is called immediately if the renderer is already enabled,
  otherwise when it becomes enabled; `render()` every frame.
  **`init` can run more than once, and when it does, every GL object you had
  is gone.** On Android the EGL context is destroyed when the app goes to the
  background, so coming back calls `init` again on a blank context: build your
  textures, buffers and shaders there, and do not skip the work the second
  time (`apps/ey3-maze-fishtank` shows the pattern).
  `render()`/`init()` run with the GL context current, so plain
  `GLES3/gl3.h` calls (any of the `#version 300 es` API) work directly.
- See `apps/ey3-triangle` (main EY3 repo) for a complete, minimal working
  example, including a shader pair under `assets/shaders/`, and
  `apps/ey3-maze` for a bigger one: a game whose objects are the
  `Renderizable`s, kept apart from the level that holds them and the rules
  that drive them.

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
  where the build leaves an `assets` symlink; Android: via `AAssetManager`,
  from what `aapt` packaged). `Shader::init` uses this for shader source.
- `cv::Mat loadImageAsset(const char* assetPath)` -- **RGB, or RGBA when the
  file has transparency** (the same order `Camera::getFrame` uses, which is
  the one GL wants; grayscale comes back as RGB). Check `image.channels()` and
  hand `GL_RGBA` to `Texture::generate` when it is 4.

## Graphics helpers: Shader, Texture, Background

- **`Shader`**
  - `void init(AssetLoader*, const char* vShaderAssetPath, const char* fShaderAssetPath)`
    -- compiles+links a `#version 300 es` vertex/fragment pair loaded from
    `assets/`.
  - `void init()` -- built-in default pass-through shader, no assets needed
    (this is how `ey3-orb-demo` gets away with no `assets/` folder at all).
  - `GLuint getProgram()`.
- **`Texture`**
  - `void generate(GLubyte* data, GLuint width, GLuint height, GLuint format = GL_RGB)`
    -- `format` is `GL_RGB` or `GL_RGBA`, and is remembered, so `update()`
    keeps using it. Pass `GL_RGBA` for an image with transparency.
  - `void bind()`, `void update(GLubyte* data)`.
  - `void invalidate()` -- the context that held this texture is gone: forget
    its name **without deleting it**, because that number may already belong
    to a texture of the new context. Call it before uploading again, and see
    `Renderer::getContextGeneration()` for knowing when.
- **`Background`** -- a ready-made `Renderizable` that fills the screen with
  a texture: `Background(GLubyte* pixels, GLuint width, GLuint height, GLuint format = GL_RGB)`.

## 3D maths: Vec3, Mat4

Only what a simple 3D app needs; there is no vector library hiding here.

- **`Vec3`** -- `{ float x, y, z; }`, plain data.
- **`Mat4`** (`<graphics/mat4.h>`) -- a 4x4 matrix stored column major, so
  `data()` goes straight into `glUniformMatrix4fv` without transposing.
  - `Mat4()` -- identity.
  - `static Mat4 translation(x, y, z)`, `scaling(x, y, z)`,
    `rotation(angleRadians, axisX, axisY, axisZ)`.
  - `static Mat4 frustum(left, right, bottom, top, near, far)` -- perspective
    from an off-centre window on the near plane. Symmetric, it is the usual
    perspective; **asymmetric, it is the projection of an eye looking through
    a window that is not in front of it**, which is what head-coupled
    perspective is made of (see `apps/ey3-maze-fishtank`).
  - `Mat4 operator*(const Mat4&)`, `const float* data()`.

## Head tracking: HeadTracker

```cpp
AssetLoader assets;
HeadTracker tracker;
tracker.init(&assets, "models/face_detection_yunet.onnx");
tracker.setCameraOffset(0.0f, 7.0f);   // the lens sits above the screen

Camera camera;                          // the front one
cv::Mat frame;
if (camera.getFrame(frame)) {
    tracker.update(frame, engine.getDeltaTime());
}
Vec3 eye = tracker.getEyePosition();    // centimetres from the centre of the screen
```

- **`HeadTracker`** (`<system/head_tracker.h>`) -- follows the player's face
  with the front camera and says where their eyes are, in centimetres, with
  the origin at the centre of the screen: +x to the player's right, +y up,
  +z towards the player. Feed that into `Mat4::frustum` and the screen stops
  being a picture and becomes a window into the scene.
  - `bool init(AssetLoader*, const char* modelAsset)` -- loads the detector's
    model, which lives in the app's `assets/`. Needs no GL context. Returns
    false if the model is missing, and then no face is ever found.
  - `void update(const cv::Mat& frame, float deltaTime)` -- the frame must be
    **RGB**, which is what `Camera::getFrame` gives.
  - `Vec3 getEyePosition()`, `bool hasFace()`, `const FaceDetection& getDetection()`
    (the box, both eyes and the score, for drawing a debug overlay), and
    `float getDetectionMillis()` -- what the last detection cost, which is
    most of the lag between moving your head and the view following.
  - `void setFieldOfView(float degrees)` (default 60, measured across the
    **longer** side of the image, because Android turns frames upright for a
    portrait screen), `setCameraOffset(xCm, yCm)`, `setEyeDistance(cm)`
    (default 6.3, the adult average, and what sets the distance scale),
    `setMirrored(bool)` (default true, as a front camera is),
    `setRestingPosition(Vec3)` -- where the viewpoint drifts back to when no
    face is visible.
  - Faces are found with **YuNet**, the small neural detector OpenCV ships:
    it gives both eyes directly, so the distance comes from how far apart
    they look, which is far steadier than the size of a head and does not
    care what colour the wall behind the player is. It needs an OpenCV built
    with `objdetect` and `dnn` (see `doc/environment-setup.md`) and the model
    file in `assets/`; get it from OpenCV's model zoo
    (`face_detection_yunet_2023mar.onnx`, 228 KB).
  - The result is smoothed with a One Euro filter, which keeps the scene from
    shaking without adding the lag an average would.
  - See `apps/ey3-maze-fishtank` for the whole thing working, debug overlay
    included.

## Camera / computer vision

OpenCV is a hard dependency of the engine, and it is not hidden behind an
opaque API: `cv::Mat` comes straight into your own code.

- **`Camera`** (`<system/camera_desktop.h>`/`camera_android.h`, same API on
  both):
  - `Camera(int32_t deviceIndex = 0)`
  - `bool open(CameraFacing facing, int32_t width, int32_t height)`,
    `void close()`
  - `bool getFrame(cv::OutputArray out)` -- **RGB**, not OpenCV's usual BGR:
    both platforms convert so the frame can go straight into a `Texture` or a
    `CameraView`. `AssetLoader::loadImageAsset` gives the same order, so
    everything in the engine speaks RGB.
  - `bool isLoaded()`, `bool isOpened()`, `int32_t getFrameWidth()/getFrameHeight()`,
    `int32_t getImageRotation()`
  - `float getFieldOfView()` -- degrees across the sensor's longer side, from
    the lens's focal length and the sensor's size, or **0 when the platform
    cannot tell** (always 0 on desktop: V4L2 says nothing about the optics).
    Anything turning pixels into angles wants this instead of a guess -- see
    `HeadTracker::setFieldOfView`.
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
