cmake_minimum_required(VERSION 3.16)
project(EY3 VERSION 0.2
	DESCRIPTION "OpenCV and OpenGL ES on Android from C++ alone, no Java and no Gradle")

# This builds the engine for Android, and nothing else: it is the check that
# the library still cross-compiles. Apps -- including the examples in apps/ --
# are separate projects that compile the engine from the installed SDK (see
# scripts/install-sdk.sh and any apps/*/android/CMakeLists.txt).
#
#   cmake -B build -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
#     -DOPENCV_DIR=$HOME/opencv-android-full/sdk/native
#   ninja -C build
#
# For the desktop library, see desktop/CMakeLists.txt; for the toolchain,
# doc/environment-setup.md.

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -Wall -fdiagnostics-color=always")

if(NOT DEFINED LOGNAME)
	set(LOGNAME "ey3")
endif()

set(EY3_SOURCE_ROOT ${PROJECT_SOURCE_DIR})
include(${PROJECT_SOURCE_DIR}/cmake/ey3-android-engine.cmake)
