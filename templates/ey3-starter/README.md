# ey3-starter

Starting point for a new project on top of the EY3 engine. Nothing is
vendored into it: both builds reference one shared, centrally-installed copy
of the engine, the same way a project depends on `glfw` or `OpenCV` without
keeping a copy around.

It is the same layout the examples in the EY3 repository use, so any of them
is a worked version of this.

`doc/ey3-api.md` here is the whole API (`Engine`, `Renderer`, `Renderizable`,
input, `Camera`/`CameraView`, `HeadTracker`, ...). Worth reading before
writing your first scene, since this directory has no engine headers to
browse.

## 1. Install ey3 (once, from the main EY3 repo)

```sh
cd /path/to/EY3          # the main repo, not this starter
./scripts/install-sdk.sh
```

- **Desktop**: builds `libey3.so` and `cmake --install`s it with its headers
  and an `ey3Config.cmake`, by default into `/usr/local` (needs `sudo`, so
  run it from a real terminal). `EY3_PREFIX=$HOME/.local` installs without
  root; then pass `-DCMAKE_PREFIX_PATH=$HOME/.local` when configuring.
- **Android**: copies the engine sources, headers and CMake modules into
  `~/.local/share/ey3-android-sdk`. There is no prebuilt equivalent — a
  static lib built with one NDK is not reliably ABI-compatible with an app
  built against another — so every Android project compiles `ey3` from that
  one shared copy.

Re-run the script whenever the engine changes; both builds pick it up on
their next build, and it also refreshes `doc/ey3-api.md` here.

## 2. Copy it

```sh
cp -r /path/to/EY3/templates/ey3-starter /path/to/my-new-project
```

Nothing inside needs adjusting to point back at the main repo. Treat the copy
as its own repo from there.

## Desktop

```sh
cmake -S . -B build -G Ninja
cmake --build build
./build/app
```

- `desktop/main_desktop.cpp` — creates the window and the `Engine`, then
  calls `runProgram` (`src/program.cpp`).
- `src/program.cpp` / `include/program.h` — your app, shared with Android.
  Right now it just clears the screen; register your own
  `ey3::Renderizable` there.
- `app.cmake` — the list of your sources, read by both builds. Adding a file
  means editing this and nothing else.
- `CMakeLists.txt` — `find_package(ey3)` and link `ey3::ey3`, which carries
  the include dirs and the glfw/EGL/GLESv2/OpenCV dependencies with it.

## Android

```sh
cmake -S android -B build-android -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DSDK_VERSION=34.0.0 -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
  -DOPENCV_DIR=/path/to/opencv-android/sdk/native
cmake --build build-android --target app-unsigned   # no keystore needed
cmake --build build-android --target app-apk        # signed
cmake --build build-android --target app-run        # installs it with adb
```

See `doc/environment-setup.md` in the main EY3 repo for how to get
`ANDROID_NDK`/`ANDROID_HOME`/`OPENCV_DIR` in place.

`android/CMakeLists.txt` includes two modules from the installed SDK — one
builds the engine, the other packages the APK — and then declares the app in
four lines. How an APK is built lives in those modules, not here, so a change
to the recipe arrives with the next `install-sdk.sh`.

Signing uses a throwaway self-signed key generated once under the build
directory: fine for your own device, not for a release. Pass your own with
`-DKEYSTORE=... -DKEYALIAS=... -DSTOREPASS=... -DKEYPASS=... -DCOMPANY=...`.

- `android/AndroidManifest.xml`, `android/res/` — rename the `package`,
  `app_name` and icons. `screenOrientation="portrait"` is a placeholder.
- `android/src/main_android.cpp` — entry point, same `runProgram` call as
  desktop.

## Window size and orientation

`MOBILE_WIDTH_PORTRAIT` / `MOBILE_HEIGHT_PORTRAIT` (and the `_LANDSCAPE`
pair) in `<system/window_desktop.h>` give an approximate phone-in-hand window
for desktop testing. Keep the pair you pick consistent with
`android/AndroidManifest.xml`'s `android:screenOrientation`.
