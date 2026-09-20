# EY3

OpenCV and OpenGL ES on Android, from C++ alone: no Java, no Gradle, no IDE.

EY3 is a small library for **quick computer-vision and graphics experiments on
a phone**. You write `main()`; it gives you a GL context, touch events, the
camera as a `cv::Mat`, and an APK at the end of `ninja`. The same code builds
and runs on the desktop, which is where most of the work happens.

It is **not a game engine**: no sound, no text rendering, no physics, no
editor. The whole API is about a dozen classes and fits in one page --
[doc/ey3-api.md](doc/ey3-api.md).

## Why you might want it

* Real-time OpenCV in C++ on Android, without fighting the Android build.
* The same source running on your machine, so you can iterate without a device.
* An APK straight from a CMake target: `aapt`, `jarsigner`, `zipalign`.
* No Android Studio, no IDE -- it builds over SSH.

If you are writing an ordinary app with views and Material, this is the wrong
tool. If you are writing a game, something like raylib will take you further.

## Installing it

Java, the Android SDK and NDK, CMake, Ninja, and an OpenCV Android SDK **built
with `objdetect` and `dnn`**; [doc/environment-setup.md](doc/environment-setup.md)
has every step.

```bash
./scripts/install-sdk.sh
```

That installs the desktop library (headers, `libey3.so` and an
`ey3Config.cmake`) and, for Android, one shared copy of the engine sources
and the CMake modules that build them. Everything else -- the examples below,
the starter, your own projects -- is a separate project that finds ey3 that
way. Nothing vendors a copy of it.

## Examples

Each one is a standalone project built against the installed library, with
its own README explaining what it shows and how to run it.

| Example | What it shows |
| --- | --- |
| [ey3-triangle](apps/ey3-triangle) | The smallest thing that draws: one `Renderizable`, a shader pair, touch input. |
| [ey3-background-img](apps/ey3-background-img) | An image loaded from `assets/` and stretched across the screen. |
| [ey3-orb-demo](apps/ey3-orb-demo) | ORB feature matching on live camera frames -- the computer-vision side. |
| [ey3-maze](apps/ey3-maze) | A 2D maze: sprite sheets, levels read from `assets/`, and a structure worth copying. |
| [ey3-maze-fishtank](apps/ey3-maze-fishtank) | The same maze in 3D under a head-coupled perspective: the front camera follows your eyes and the screen becomes a window into a box. What the library was built for. |

## Starting your own project

Copy [templates/ey3-starter](templates/ey3-starter) anywhere outside this
repository and build it -- its README has the details. It is the same layout
every example uses: your code in `src/` and `include/`, the two entry points
in `desktop/` and `android/`, and one `app.cmake` both builds read.

## Working on the library itself

```bash
cmake -S desktop -B build-desktop -G Ninja && cmake --build build-desktop
```

builds the desktop library, and the root `CMakeLists.txt` cross-compiles the
same engine for Android, which is the check that it still builds there. Both
are only for developing ey3; apps never use them directly.

## Licence

EY3 is Apache 2.0 -- see [LICENSE](LICENSE). It is built on OpenCV
(https://opencv.org, Apache 2.0); [doc/third-party.md](doc/third-party.md)
lists everything it uses and what shipping a build of it involves.
