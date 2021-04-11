# EY3 (project under developpement)
A simple library to facilitate the use of OpenCV and OpenGL in Android using only c++.

## Usage
You may find useful this library if you are interested in the following characteristics:
* Easy developement with OpenCV in c++ for Android (specially useful for high-performance operations in real-time) without struggling too much in Android configurations.
* Build an app with [native_app_glue](https://developer.android.com/ndk/samples/sample_na "android_app_glue").
* Minimum use of Java or Kotlin code and views, just c++ and OpenGL.
* Build an app without Android Studio or an IDE.
* Games from scratch.

You shouldn't use this library if you plan to build a typical app with java, material, views and all of these stuff. This has been created mainly to made quick experiments in Android with OpenCV and OpenGL.

## Pre-requisites
* [Java 8+](https://www.java.com)
* [Android SDK 29+](https://developer.android.com/studio)
* [Android NDK r21+](https://developer.android.com/ndk/)
* Make sure you have the following environement variables:
 * JAVA_HOME
 * ANDROID_HOME
* [cmake](https://cmake.org)
* [ninja](https://ninja-build.org/)
* [OpenCV 4+](https://opencv.org)
 * Use cmake to configure OpenCV build for android with everything you need. You can modify the following command, replacing the content between brackets:
```bash
    cmake [OPENCV SOURCE] -B [OPENCV BUILD OUTPUT] -DBUILD_opencv_ittnotify=OFF -DBUILD_ITT=OFF -DCV_DISABLE_OPTIMIZATION=ON -DWITH_TBB=ON -DANDROID_ARM_NEON=ON -DWITH_CUDA=OFF -DWITH_OPENCL=ON -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_VA_INTEL=OFF -DCPU_BASELINE_DISABLE=ON -DENABLE_SSE=OFF -DENABLE_SSE2=OFF -DBUILD_TESTING=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_opencv_apps=OFF -DBUILD_SHARED_LIBS=OFF -DOpenCV_STATIC=ON -DWITH_1394=OFF -DWITH_ARITH_DEC=OFF -DWITH_ARITH_ENC=OFF -DWITH_CUBLAS=OFF -DWITH_CUFFT=OFF -DWITH_FFMPEG=OFF -DWITH_GDAL=OFF -DWITH_GSTREAMER=OFF -DWITH_GTK=OFF -DWITH_HALIDE=OFF -DWITH_JASPER=OFF -DWITH_NVCUVID=OFF -DWITH_OPENEXR=OFF -DWITH_PROTOBUF=OFF -DWITH_PTHREADS_PF=OFF -DWITH_QUIRC=OFF -DWITH_V4L=OFF -DWITH_WEBP=OFF -DBUILD_LIST=core,features2d,flann,imgcodecs,imgproc,stitching -DANDROID_NDK=[ANDROID NDK PATH] -DCMAKE_TOOLCHAIN_FILE=[ANDROID NDK PATH]/build/cmake/android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=android-29 -DBUILD_JAVA=OFF -DBUILD_ANDROID_EXAMPLES=OFF -DBUILD_ANDROID_PROJECTS=OFF -DANDROID_STL=c++_shared -DCMAKE_INSTALL_PREFIX:PATH=[OPENCV BUILD OUTPUT] -DANDROID_ABI=[YOUR TARGET ABI] -G Ninja
```
 * Build it:
`ninja .`
* Android device running android-19+

## Getting started
1. Use cmake to configure EY3 build, passing the following arguments:
 * -DCMAKE_TOOLCHAIN_FILE: Path to the [NDK_PATH]/build/cmake/android.toolchain.cmake
 * -DSDK_VERSION: SDK version to use (i.e. 29.0.1)
 * -DANDROID_PLATFORM: your android target platform (i.e. android-29)
 * -DANDROID_ABI: your target android abi (i.e. arm64-v8a)
 * -DOPENCV_DIR: path to your built OpenCV path + /sdk/native

 Optional arguments:
 * -DLOGNAME: name to use for logging

 Optional arguments to sign sample apks:
 * -DKEYSTORE: keystore to sign your app
 * -DSTOREPASS: keystore password
 * -DKEYALIAS: key alias
 * -DKEYPASS: key password
 * -DCOMPANY: your company name

2. Compile it with ninja:
```bash
cd [cmake output]
ninja ey3
```
 It will generate the static libs **libey3.a** and **libnative_app_glue.a**, which you can add and link to your project to start building your native app.
 You can find some examples in [apps](ey3/tree/master/apps) folder. To build them, use ninja with the following targets:
 * [lib name]-unsigned: generates the unsigned apk
 * [lib name]-apk: generates signed apk
 * [lib name]-run: installs the apk to the current device

 where [lib name] refers to an available sample app:
 *  [ey3-triangle](ey3/tree/master/apps/ey3-triangle): an example that displays a triangle in the middle of the screen that changes its color depending on the user touch region.
 *  [ey3-background-img](ey3/tree/master/apps/ey3-background-img): an example that loads an image from the assets folder and displays it streched to the screen.
 *  [ey3-orb-demo](ey3/tree/master/ey3-orb-demo): an example of feature matching with ORB in real-time.


## External libraries
This software uses the following external libraries:

- OpenCV: Licensed under [Apache 2](https://github.com/opencv/opencv/blob/master/LICENSE) license
  Website: https://opencv.org