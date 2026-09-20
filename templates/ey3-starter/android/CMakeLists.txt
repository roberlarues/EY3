# Android build for a project using ey3.
#
# The engine is not vendored into this project: it is compiled from ONE shared
# copy of its sources, installed by scripts/install-sdk.sh in the main EY3
# repository (default ~/.local/share/ey3-android-sdk). The same copy brings
# the two CMake modules included below, so how the engine is built and how an
# APK is packaged live in one place rather than being copied into every
# project. Refresh that copy and this project picks the change up on its next
# build.
#
# Usage (see ../README.md for how to obtain ANDROID_NDK / OPENCV_DIR):
#   cmake -S android -B build-android -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DSDK_VERSION=34.0.0 -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
#     -DOPENCV_DIR=$HOME/opencv-android-full/sdk/native
#   cmake --build build-android --target app-unsigned   # no keystore needed
#   cmake --build build-android --target app-apk        # signed; see below
#   cmake --build build-android --target app-run        # installs it with adb
#
# Signing uses a throwaway keystore with default passwords unless you pass
# -DKEYSTORE=... -DKEYALIAS=... -DSTOREPASS=... -DKEYPASS=... -DCOMPANY=...
cmake_minimum_required(VERSION 3.16)
project(EY3App)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -Wall")

# Where the shared copy of ey3 lives.
if(NOT DEFINED EY3_ANDROID_SDK)
	set(EY3_ANDROID_SDK "$ENV{HOME}/.local/share/ey3-android-sdk")
endif()
if(NOT EXISTS ${EY3_ANDROID_SDK})
	message(FATAL_ERROR "EY3_ANDROID_SDK (${EY3_ANDROID_SDK}) not found -- run "
		"install-sdk.sh in the main EY3 repository first, or pass "
		"-DEY3_ANDROID_SDK=/path/to/it.")
endif()

# The tag this app's logs are written under.
set(LOGNAME "app")

set(EY3_SOURCE_ROOT ${EY3_ANDROID_SDK})
include(${EY3_ANDROID_SDK}/cmake/ey3-android-engine.cmake)
include(${EY3_ANDROID_SDK}/cmake/ey3-android-app.cmake)

# Everything but the entry point is shared with the desktop build.
set(APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
include(${APP_ROOT}/app.cmake)
list(TRANSFORM APP_SOURCES PREPEND ${APP_ROOT}/)

ey3_add_android_app(app
	SOURCES src/main_android.cpp ${APP_SOURCES}
	INCLUDES ${APP_ROOT}/include
	ASSETS ${APP_ROOT}/assets)
