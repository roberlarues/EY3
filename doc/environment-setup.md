# Development environment: installation

How to set up a machine that can build EY3 from the command line, without
Android Studio. It covers two scenarios that must end in the same state:

- **Local development**: Arch Linux.
- **CI**: Ubuntu (an Ubuntu LTS runner, most likely).

The Android SDK/NDK are installed through `sdkmanager` (the official,
cross-platform tool) rather than from distribution packages, so that the
installed versions are *identical* on both systems. Only the base tools
(Java, cmake, ninja, utilities) come from each distribution's package
manager.

## 1. Versions

| Tool | Recommended version | Why |
|---|---|---|
| JDK | 17 | Required by recent Android `cmdline-tools`; also provides `keytool`/`jarsigner`. |
| cmake | >= 3.22 (the system one) | Build generator, for both EY3 and OpenCV. |
| ninja | >= 1.10 (the system one) | Build backend, used by every `add_custom_command` in the project. |
| Android cmdline-tools | latest | Installs the SDK/NDK/build-tools reproducibly through `sdkmanager`. |
| Android build-tools | 34.0.0 (or the latest stable) | `aapt` and `zipalign`, used by `cmake/ey3-android-app.cmake` to package an APK. |
| Android platform | android-24 | The code's real minimum: it uses the Camera2 NDK (`ACameraManager_create`) and `AImageReader`, both available from API 24. |
| Android NDK | r27 (LTS) | The C++ toolchain for Android. Avoid release candidates. |
| OpenCV | a recent 4.x, with `objdetect` and `dnn` | A hard dependency of the engine (see §5). |

## 2. Base distribution packages

### Arch Linux (local development)

```bash
sudo pacman -S --needed jdk17-openjdk cmake ninja unzip wget git base-devel
```

### Ubuntu (CI pipeline)

```bash
sudo apt-get update
sudo apt-get install -y openjdk-17-jdk cmake ninja-build unzip wget git build-essential
```

`build-essential`/`base-devel` are needed because `cmake` runs native
compiler checks (`try_compile`) while configuring, even though the final EY3
build is cross-compiled with the NDK.

## 3. Android SDK (cmdline-tools)

The same procedure on Arch and Ubuntu, since it downloads Google's official
zip instead of using the distribution's package manager.

1. Download the *Command line tools only* package from the official page:
   https://developer.android.com/studio#command-tools
2. Unzip it into this exact structure (the tool is fussy about the
   `cmdline-tools/latest/` path):

```bash
mkdir -p $HOME/android-sdk/cmdline-tools
unzip commandlinetools-linux-*.zip -d $HOME/android-sdk/cmdline-tools
mv $HOME/android-sdk/cmdline-tools/cmdline-tools $HOME/android-sdk/cmdline-tools/latest
```

3. Export the environment variables (add them to `~/.bashrc` / `~/.zshrc` or
   to the CI script):

```bash
export ANDROID_HOME="$HOME/android-sdk"
export JAVA_HOME="/usr/lib/jvm/java-17-openjdk"   # adjust per distribution
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"
```

   On Ubuntu, `JAVA_HOME` is usually `/usr/lib/jvm/java-17-openjdk-amd64`.
   These variables are read **while configuring** (see §7.1).

4. Accept the licences and install the packages:

```bash
yes | sdkmanager --sdk_root="$ANDROID_HOME" --licenses
sdkmanager --sdk_root="$ANDROID_HOME" \
  "platform-tools" \
  "platforms;android-34" \
  "platforms;android-24" \
  "build-tools;34.0.0"
```

> Install **both** platforms, not just 34: the build uses the same
> `ANDROID_PLATFORM` variable for the NDK's API level
> (`-DANDROID_PLATFORM=android-24`, see §6) and for the `android.jar` that
> `aapt` packages against (`platforms/<ANDROID_PLATFORM>/android.jar`).

   Run `sdkmanager --sdk_root="$ANDROID_HOME" --list` to check for a newer
   `build-tools`/`platform` than the ones in the table.

## 4. Android NDK

```bash
sdkmanager --sdk_root="$ANDROID_HOME" --list | grep ndk
# pick the latest r27.x (avoid "rc" / release candidates)
sdkmanager --sdk_root="$ANDROID_HOME" "ndk;27.2.12479018"
```

```bash
export ANDROID_NDK="$ANDROID_HOME/ndk/27.2.12479018"   # match what you installed
```

(The exact patch number may have moved on since this was written -- use
whatever `sdkmanager --list` offers.)

## 5. OpenCV for Android (required)

The quick way is the official Android SDK, which ships the static `.a` files
already built and in the layout EY3 expects (`staticlibs/<ABI>/`,
`3rdparty/libs/<ABI>/`, `jni/include/`):

```bash
curl -LO https://github.com/opencv/opencv/releases/download/4.10.0/opencv-4.10.0-android-sdk.zip
unzip -q opencv-4.10.0-android-sdk.zip -d ~/tmp && mv ~/tmp/OpenCV-android-sdk ~/opencv-android-full
export OPENCV_BUILD_OUTPUT="$HOME/opencv-android-full"
```

`$OPENCV_BUILD_OUTPUT/sdk/native` is what you pass as `-DOPENCV_DIR` when
configuring an Android build.

> **It needs `core`, `imgproc`, `imgcodecs`, `objdetect` and `dnn`** (plus
> `features2d`, `calib3d` and `flann` for `ey3-orb-demo`). `HeadTracker` uses
> YuNet, which lives in `objdetect` and runs on `dnn`: a trimmed build
> without those two compiles everything else but not the head tracking. If
> you build OpenCV from source instead of using the official SDK, add them to
> `-DBUILD_LIST` and install with `CMAKE_INSTALL_PREFIX` pointing at
> `$OPENCV_BUILD_OUTPUT` -- but expect it to be by far the slowest step of
> the whole setup, and cache that directory between CI builds.

## 6. Building

The library and the apps are separate builds. Install the library once:

```bash
./scripts/install-sdk.sh          # EY3_PREFIX=$HOME/.local to install without root
```

and then build any app -- the examples in `apps/`, `templates/ey3-starter`,
or your own -- from its own directory:

```bash
cd apps/ey3-maze-fishtank
cmake -S android -B build-android -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DSDK_VERSION=34.0.0 \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_ABI=arm64-v8a \
  -DOPENCV_DIR="$OPENCV_BUILD_OUTPUT/sdk/native"
cmake --build build-android --target ey3-maze-fishtank-unsigned
```

The `-unsigned` target needs no keystore and is enough to check that
everything compiles and packages. `-apk` (signed) and `-run` (install over
`adb`) only make sense with a keystore configured and, for `-run`, a device
attached -- which a CI runner normally has neither of.

For the library itself, the root `CMakeLists.txt` cross-compiles the engine
for Android without building any app, which is the cheapest check that a
change still builds there:

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
  -DOPENCV_DIR="$OPENCV_BUILD_OUTPUT/sdk/native"
ninja -C build
```

`ey3-triangle` is the example to build when checking a new environment end to
end: if it packages, the core (engine, renderer, EGL/GLES3, delta time) is
sound. `ey3-maze-fishtank` additionally proves `objdetect`/`dnn`, since it is
the only one using `HeadTracker`.

> Compiling and packaging is not the same as validating at runtime. The
> camera and the head tracking can only really be checked on a device: CI can
> guarantee the project builds on every change, it does not replace a manual
> test before calling a release good.

## 7. Environment variables -- summary for the pipeline

```bash
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64   # Ubuntu path
export ANDROID_HOME=$HOME/android-sdk
export ANDROID_NDK=$ANDROID_HOME/ndk/27.2.12479018
export OPENCV_BUILD_OUTPUT=$HOME/opencv-android-full   # SDK with objdetect and dnn
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"
```

### 7.1 The variables are read while configuring, not while building

`ANDROID_HOME` and `JAVA_HOME` are looked up when you run `cmake -B build`,
not on every compile. Exporting them afterwards fixes nothing: configure
again from a shell that already has them.

Configuring without them fails immediately, with a message naming what is
missing -- `cmake/ey3-android-app.cmake` resolves `aapt`, `zipalign`,
`keytool` and `jarsigner` with `find_program` and checks that the
`android.jar` exists.
