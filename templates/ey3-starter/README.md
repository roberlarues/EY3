# ey3-starter

Starting point for a new project on top of the EY3 engine. It doesn't vendor
a copy of ey3 into itself -- both platforms reference one shared,
centrally-installed copy of the engine, the same way this project already
depends on `glfw`/`OpenCV` without keeping a copy of them around.

See `doc/ey3-api.md` for what the engine actually offers (`Engine`,
`Renderer`, `Renderizable`, input, `Camera`/`CameraView`, ...) -- worth
reading before writing your own scene, since this directory doesn't have the
engine's headers sitting around to browse.

## 1. Install ey3 (once, from the main EY3 repo)

```sh
cd /path/to/EY3          # the main repo, not this starter
./scripts/install-sdk.sh
```

This does two independent things:

- **Desktop**: builds `libey3.so` and `cmake --install`s it, with its
  headers and a `ey3Config.cmake`, into a prefix -- default `/usr/local`
  (needs `sudo`; run it from a real terminal, not through an AI coding
  agent that has no TTY to prompt you for the password). Pass
  `EY3_PREFIX=$HOME/.local` to install without root -- then any project
  using it needs `-DCMAKE_PREFIX_PATH=$HOME/.local` at configure time (see
  below), since that path usually isn't on CMake's default search path.
- **Android**: copies the engine sources + headers into ONE shared location,
  default `~/.local/share/ey3-android-sdk` (no root needed). There's no
  installable prebuilt equivalent here -- see the Android section below for
  why -- so every Android project compiles `ey3` from this one shared copy
  instead of vendoring its own.

Re-run this script any time `include/`, `src/` or `doc/ey3-api.md` change in
the main repo, to refresh both installs and this starter's copy of the API
doc (see "Updating ey3" below).

## 2. Create a new project from this starter

```sh
cp -r /path/to/EY3/templates/ey3-starter /path/to/my-new-project
cd /path/to/my-new-project
```

That's it -- nothing here needs adjusting to point back at the main repo,
because step 1 already installed ey3 somewhere both this copy and any other
project can find it. From here, treat `/path/to/my-new-project` as its own
independent repo (e.g. `git init` it) and build it as described below.

## Desktop (Linux)

```sh
cmake -S . -B build -G Ninja
cmake --build build
./build/app
```

`find_package(ey3)` in `CMakeLists.txt` picks up the install from step 1
automatically if it went to the default `/usr/local`. If you installed
elsewhere (`EY3_PREFIX=...`), add `-DCMAKE_PREFIX_PATH=/that/prefix` to the
`cmake -S` line above.

- `desktop/main_desktop.cpp` -- desktop entry point: creates the window +
  `Engine`, then calls into `runProgram` (`src/program.cpp`).
- `src/program.cpp` / `include/program.h` -- your app logic, shared between
  desktop and Android (same pattern as `apps/ey3-triangle` in the main
  repo). Right now it's an empty scene (nothing rendered, just clears the
  screen) -- add your own `ey3::Renderizable` and register it there; see
  `apps/ey3-triangle` for a minimal working example.
- `CMakeLists.txt` -- `find_package(ey3)` + link `ey3::ey3`. That target
  already carries ey3's own include dirs and its glfw/EGL/GLESv2/OpenCV
  dependencies, so a custom `Renderizable` calling GLES functions or
  `Camera`/`CameraView` just works, no extra `find_package` needed on your
  side.

## Android

```sh
cd android
cmake -S . -B build-android -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DSDK_VERSION=34.0.0 -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
  -DOPENCV_DIR=/path/to/opencv-android/sdk/native
cmake --build build-android --target app-unsigned   # no keystore needed, just to validate the build
cmake --build build-android --target app-apk        # signed with a throwaway debug keystore (see CMakeLists.txt)
$ANDROID_HOME/platform-tools/adb install -r build-android/app.apk
```

See `../doc/environment-setup.md` in the main EY3 repo for how to get
`ANDROID_NDK`/`ANDROID_HOME`/`OPENCV_DIR` set up in the first place.

There's no installable prebuilt `ey3` for Android the way there is for
desktop: a static lib built with one NDK/libc++ version isn't reliably
ABI-compatible with an app built against a different NDK version, so
`CMakeLists.txt` instead compiles `ey3` from source as part of your own app's
build -- from `EY3_ANDROID_SDK` (default `~/.local/share/ey3-android-sdk`,
what step 1 populated), **not** a copy inside this project. Pass
`-DEY3_ANDROID_SDK=/other/path` if you installed it elsewhere.

- `AndroidManifest.xml`, `res/` -- rename the `package`, `app_name` and icons
  to your own. `screenOrientation="portrait"` is a placeholder; change it (or
  drop it entirely to allow rotation) to match your app.
- `src/main_android.cpp` -- Android entry point, same `runProgram` call as
  desktop's `main_desktop.cpp`.
- `CMakeLists.txt` -- compiles `ey3` from `EY3_ANDROID_SDK`, links it (plus
  `src/program.cpp`) into `libapp.so`, strips debug info, and packages/signs
  the APK (`aapt` / `jarsigner` / `zipalign`, same recipe as the main repo).

The `KEYSTORE`/`STOREPASS`/`KEYALIAS`/`KEYPASS`/`COMPANY` cache variables in
`CMakeLists.txt` default to a throwaway self-signed debug key generated once
under the build directory -- fine for testing on your own device, not for a
real release (pass your own via `-D...` for that).

## Updating ey3

The engine *will* change -- new methods, bugfixes, maybe new modules. When it
does, from the main EY3 repo:

```sh
./scripts/install-sdk.sh
```

Then, per project using this starter:

- **Desktop**: just rebuild (`cmake --build build`) -- it links the shared
  `libey3.so` dynamically, so a new build of your app picks up the new
  version automatically, no copying involved. If the change added/renamed a
  public method or type you use, you'll get a normal compile error pointing
  at it, same as any other dependency update.
- **Android**: also just rebuild -- `EY3_ANDROID_SDK` points at the shared
  source checkout `install-sdk.sh` just refreshed, so the next build
  recompiles `ey3` from the updated sources.
- If the change is to the *build recipe itself* (e.g. a new OpenCV component
  ey3 now needs, a new system lib to link) rather than just the engine's own
  code, `CMakeLists.txt` here (and `android/CMakeLists.txt`) need the
  matching update by hand -- diff them against `desktop/CMakeLists.txt` /
  the root `CMakeLists.txt` in the main repo if something stops linking
  after an update.
- `doc/ey3-api.md` is refreshed by the same `install-sdk.sh` run, from
  `doc/ey3-api.md` in the main repo.

## Window size / orientation

`MOBILE_WIDTH_PORTRAIT` / `MOBILE_HEIGHT_PORTRAIT` (and the `_LANDSCAPE`
pair) in `<system/window_desktop.h>` give you an approximate mobile-in-hand
window size for desktop testing. Pick the pair matching your app's intended
orientation -- and keep it consistent with `android/AndroidManifest.xml`'s
`android:screenOrientation`.
