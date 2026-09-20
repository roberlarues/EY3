# The same maze in 3D, seen through a head-coupled perspective.
#
# Android build. The engine is not vendored here: it is compiled from the one
# shared copy that scripts/install-sdk.sh leaves in EY3_ANDROID_SDK, which
# also brings the two CMake modules included below -- how the engine is built
# and how an APK is packaged live there, not in every project.
#
#   cmake -S android -B build-android -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DSDK_VERSION=34.0.0 -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
#     -DOPENCV_DIR=$HOME/opencv-android-full/sdk/native
#   cmake --build build-android --target ey3-maze-fishtank-unsigned   # no keystore needed
#   cmake --build build-android --target ey3-maze-fishtank-apk        # signed
#   cmake --build build-android --target ey3-maze-fishtank-run        # installs it with adb
cmake_minimum_required(VERSION 3.16)
project(ey3-maze-fishtank-android)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -Wall")

if(NOT DEFINED EY3_ANDROID_SDK)
	set(EY3_ANDROID_SDK "$ENV{HOME}/.local/share/ey3-android-sdk")
endif()
if(NOT EXISTS ${EY3_ANDROID_SDK})
	message(FATAL_ERROR "EY3_ANDROID_SDK (${EY3_ANDROID_SDK}) not found -- run "
		"scripts/install-sdk.sh in the EY3 repository first, or pass "
		"-DEY3_ANDROID_SDK=/path/to/it.")
endif()

# The tag this app's logs are written under.
set(LOGNAME "ey3-maze-fishtank")

set(EY3_SOURCE_ROOT ${EY3_ANDROID_SDK})
include(${EY3_ANDROID_SDK}/cmake/ey3-android-engine.cmake)
include(${EY3_ANDROID_SDK}/cmake/ey3-android-app.cmake)

# Everything but the entry point is shared with the desktop build.
set(APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${APP_ROOT}/app.cmake)
list(TRANSFORM APP_SOURCES PREPEND ${APP_ROOT}/)

ey3_add_android_app(ey3-maze-fishtank
	SOURCES src/main_android.cpp ${APP_SOURCES}
	INCLUDES ${APP_ROOT}/include
	ASSETS ${APP_ROOT}/assets)
