# Standalone Android (NDK) build. Unlike ../CMakeLists.txt (desktop, which
# links a prebuilt libey3.so via find_package(ey3)), this compiles ey3 from
# source -- a static lib built with one NDK/libc++ version isn't reliably
# ABI-compatible with an app built against a different NDK version, so there
# is no installable prebuilt equivalent for Android.
#
# What it does NOT do is vendor a copy of that source into every project:
# EY3_ANDROID_SDK below points at ONE shared checkout of the engine sources
# (installed by ../../scripts/install-sdk.sh, default
# ~/.local/share/ey3-android-sdk), which this file just compiles from,
# in-place, as part of the app's own build. Update that one shared copy and
# every Android project using it picks up the change on its next build.
#
# Usage (see ../README.md for how to obtain ANDROID_NDK/OPENCV_DIR):
#   cmake -S android -B build-android -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DSDK_VERSION=34.0.0 -DANDROID_PLATFORM=android-24 -DANDROID_ABI=arm64-v8a \
#     -DOPENCV_DIR=/path/to/opencv-android/sdk/native
#   cmake --build build-android --target app-unsigned   # no keystore needed
#   cmake --build build-android --target app-apk        # signed, needs a keystore (see below)
cmake_minimum_required(VERSION 3.6.0)
project(EY3App)

if(NOT DEFINED EY3_ANDROID_SDK)
	set(EY3_ANDROID_SDK "$ENV{HOME}/.local/share/ey3-android-sdk")
endif()
if(NOT EXISTS ${EY3_ANDROID_SDK})
	message(FATAL_ERROR "EY3_ANDROID_SDK (${EY3_ANDROID_SDK}) not found -- run install-sdk.sh in the main EY3 repo first, or pass -DEY3_ANDROID_SDK=/path/to/it.")
endif()

# Debug self-signed keystore, generated once under CMAKE_BINARY_DIR. Override
# these (-DKEYSTORE=... etc.) for a real release signing key.
if(NOT DEFINED KEYSTORE)
	set(KEYSTORE "release")
endif()
if(NOT DEFINED STOREPASS)
	set(STOREPASS "changeme")
endif()
if(NOT DEFINED KEYALIAS)
	set(KEYALIAS "alias")
endif()
if(NOT DEFINED KEYPASS)
	set(KEYPASS "changeme")
endif()
if(NOT DEFINED COMPANY)
	set(COMPANY "company")
endif()

option(EY3_WITH_CV "Enable the OpenCV / Camera2 computer vision module (Camera, CameraView)" ON)

set(LIBNAME "app")
set(BUILD_TOOLS "$ENV{ANDROID_HOME}/build-tools/${SDK_VERSION}")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -Wall")

# --- ey3 engine, compiled from the shared SDK's sources ---
set(EY3_SOURCES
	${EY3_ANDROID_SDK}/src/graphics/renderer.cpp
	${EY3_ANDROID_SDK}/src/graphics/shader.cpp
	${EY3_ANDROID_SDK}/src/graphics/texture.cpp
	${EY3_ANDROID_SDK}/src/system/engine.cpp
	${EY3_ANDROID_SDK}/src/system/cmd_handler.cpp
	${EY3_ANDROID_SDK}/src/system/input_handler.cpp
	${EY3_ANDROID_SDK}/src/views/background.cpp
	${EY3_ANDROID_SDK}/src/system/window_android.cpp
	${EY3_ANDROID_SDK}/src/system/asset_loader_android.cpp
	${EY3_ANDROID_SDK}/src/system/platform_context.cpp)

if(EY3_WITH_CV)
	list(APPEND EY3_SOURCES
		${EY3_ANDROID_SDK}/src/system/camera_android.cpp
		${EY3_ANDROID_SDK}/src/views/camera_view.cpp)
endif()

add_library(ey3 STATIC ${EY3_SOURCES})
target_include_directories(ey3 PUBLIC
	${ANDROID_NDK}/sources/android/native_app_glue
	${EY3_ANDROID_SDK}/include/graphics
	${EY3_ANDROID_SDK}/include/system
	${EY3_ANDROID_SDK}/include/views
	${EY3_ANDROID_SDK}/include)
target_compile_definitions(ey3 PUBLIC LOGNAME="${LIBNAME}")

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

find_library(log-lib log)

if(EY3_WITH_CV)
	target_compile_definitions(ey3 PUBLIC EY3_WITH_CV)
	include_directories(SYSTEM ${OPENCV_DIR}/jni/include)

	add_library(cv_core STATIC IMPORTED)
	set_target_properties(cv_core PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_core.a)
	add_library(cv_imgproc STATIC IMPORTED)
	set_target_properties(cv_imgproc PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_imgproc.a)
	add_library(cv_imgcodecs STATIC IMPORTED)
	set_target_properties(cv_imgcodecs PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_imgcodecs.a)

	add_library(cv_jpeg-turbo STATIC IMPORTED)
	set_target_properties(cv_jpeg-turbo PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/liblibjpeg-turbo.a)
	add_library(cv_openjp2 STATIC IMPORTED)
	set_target_properties(cv_openjp2 PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/liblibopenjp2.a)
	add_library(cv_png STATIC IMPORTED)
	set_target_properties(cv_png PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/liblibpng.a)
	add_library(cv_tiff STATIC IMPORTED)
	set_target_properties(cv_tiff PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/liblibtiff.a)
	add_library(cv_tbb STATIC IMPORTED)
	set_target_properties(cv_tbb PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/libtbb.a)
endif()

target_link_libraries(ey3
	android
	${log-lib}
	EGL
	GLESv3
	z)
if(EY3_WITH_CV)
	target_link_libraries(ey3
		mediandk
		camera2ndk
		cv_imgcodecs
		cv_imgproc
		cv_core
		cv_jpeg-turbo
		cv_openjp2
		cv_png
		cv_tiff
		cv_tbb)
endif()

# --- your app ---
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib/${ANDROID_ABI})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib/${ANDROID_ABI})

add_library(${LIBNAME} SHARED src/main_android.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/program.cpp)
target_include_directories(${LIBNAME} PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
target_include_directories(${LIBNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${LIBNAME} android native_app_glue ey3)

# The NDK toolchain adds -g even in Release; strip it so aapt doesn't bundle
# full debug info straight into the APK (can otherwise be 10x+ larger).
add_custom_command(TARGET ${LIBNAME} POST_BUILD
	COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${LIBNAME}>
	COMMENT "Stripping debug info from ${LIBNAME}")

# --- APK packaging (aapt / jarsigner / zipalign), same recipe as the main
# EY3 repo's apps/*/CMakeLists.txt ---
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore
	COMMAND $ENV{JAVA_HOME}/bin/keytool -genkeypair -validity 10000 -keystore ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore -alias ${KEYALIAS} -keyalg RSA -keysize 2048 -storepass ${STOREPASS} -keypass ${KEYPASS} -dname "CN=${COMPANY},O=Android,C=ES"
	COMMENT "Creating keystore")
add_custom_target(keystore DEPENDS ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore)

add_custom_command(OUTPUT ${LIBNAME}.unsigned.apk
	COMMAND ${CMAKE_COMMAND} -E remove ${LIBNAME}.unsigned.apk
	COMMAND ${BUILD_TOOLS}/aapt package -f -M ${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml --auto-add-overlay -S ${CMAKE_CURRENT_SOURCE_DIR}/res -I $ENV{ANDROID_HOME}/platforms/${ANDROID_PLATFORM}/android.jar -F ${LIBNAME}.unsigned.apk .
	DEPENDS ${LIBNAME}
	COMMENT "Building apk")
add_custom_target(${LIBNAME}-unsigned DEPENDS ${LIBNAME}.unsigned.apk COMMENT "Creating unsigned APK.")

add_custom_command(OUTPUT ${LIBNAME}.signed.apk
	COMMAND jarsigner -keystore ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore -sigalg SHA256withRSA -digestalg SHA-256 -storepass ${STOREPASS} -keypass ${KEYPASS} -signedjar ${LIBNAME}.signed.apk ${LIBNAME}.unsigned.apk ${KEYALIAS}
	DEPENDS ${LIBNAME}.unsigned.apk ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore
	COMMENT "Signing APK")

add_custom_command(OUTPUT ${LIBNAME}.apk
	COMMAND ${BUILD_TOOLS}/zipalign -f 4 ${LIBNAME}.signed.apk ${LIBNAME}.apk
	DEPENDS ${LIBNAME}.signed.apk
	COMMENT "Calling zipalign")
add_custom_target(${LIBNAME}-apk DEPENDS ${LIBNAME}.apk COMMENT "Creating release APK.")

add_custom_target(${LIBNAME}-run
	COMMAND $ENV{ANDROID_HOME}/platform-tools/adb install -r ${LIBNAME}.apk
	DEPENDS ${LIBNAME}.apk
	COMMENT "Launching ${LIBNAME} APK")
