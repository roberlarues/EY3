# Third-party code and assets

EY3 itself is Apache 2.0 (see [LICENSE](../LICENSE)). What it uses, and what
that means when you ship something built with it.

| What | Where it comes from | Licence | How it reaches a build |
| --- | --- | --- | --- |
| OpenCV | opencv.org | Apache 2.0 | **Linked statically into every APK**, dynamically on desktop. |
| `android_native_app_glue` | the Android NDK (`sources/android/`) | Apache 2.0 | Compiled from the NDK on each build; no copy is kept here. |
| GLFW | the system package | zlib/libpng | Desktop only, linked dynamically. |
| `face_detection_yunet.onnx` | [OpenCV Zoo](https://github.com/opencv/opencv_zoo/tree/main/models/face_detection_yunet) | MIT, (c) 2020 Shiqi Yu | The only third-party file **stored in this repository**, in `apps/ey3-maze-fishtank/assets/models/`, with its licence beside it. Both are packaged into that app's APK. |

Everything else -- the engine, the examples, their sprites, shaders, levels
and launcher icons -- was written for this project.

## If you distribute an APK

Apache 2.0 (OpenCV, `native_app_glue`) asks that a copy of the licence and
any NOTICE travel with the binary they are compiled into. An APK built here
does not carry them on its own: add OpenCV's `LICENSE` to your app's
`assets/` (or to an about screen) before publishing one. Nothing in the build
does this for you, deliberately -- an experiment being side-loaded onto your
own phone is not distribution, and a published app usually wants its notices
somewhere a user can actually read them.

The YuNet model is the exception already handled: `assets/models/LICENSE`
sits next to it and is packaged with it.
