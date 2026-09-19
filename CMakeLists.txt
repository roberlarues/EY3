cmake_minimum_required(VERSION 3.6.0)
project(EY3 VERSION 0.1	DESCRIPTION "A simple library to facilitate the use of OpenCV and OpenGL in Android using only c++")

# USER DEFINED VARS
if(NOT DEFINED LOGNAME)
	set(LOGNAME "ey3")
endif(NOT DEFINED LOGNAME)	

if(NOT DEFINED KEYSTORE)
	set(KEYSTORE "release")
endif(NOT DEFINED KEYSTORE)

if(NOT DEFINED STOREPASS)
	set(STOREPASS "changeme")
endif(NOT DEFINED STOREPASS)

if(NOT DEFINED KEYALIAS)
	set(KEYALIAS "alias")
endif(NOT DEFINED KEYALIAS)

if(NOT DEFINED KEYPASS)
	set(KEYPASS "changeme")
endif(NOT DEFINED KEYPASS)

if(NOT DEFINED COMPANY)
	set(COMPANY "company")
endif(NOT DEFINED COMPANY)

option(EY3_WITH_CV "Enable the OpenCV / Camera2 computer vision module (Camera, CameraView)" ON)

#END USER DEFINED VARS


set(LIBNAME "ey3")
set(BUILD_TOOLS "$ENV{ANDROID_HOME}/build-tools/${SDK_VERSION}")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -Wall -fdiagnostics-color=always")


# EY3 source
set(EY3_ROOT ${PROJECT_SOURCE_DIR})
include(${PROJECT_SOURCE_DIR}/cmake/sources.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/strip.cmake)

set(EY3_SOURCES
	${EY3_SHARED_SOURCES}
	src/system/window_android.cpp
	src/system/asset_loader_android.cpp
	src/system/platform_context.cpp)

if(EY3_WITH_CV)
	list(APPEND EY3_SOURCES src/system/camera_android.cpp)
endif(EY3_WITH_CV)

add_library(${LIBNAME} ${EY3_SOURCES})

target_include_directories(${LIBNAME} PUBLIC
	${ANDROID_NDK}/sources/android/native_app_glue
	${PROJECT_SOURCE_DIR}/include/graphics
	${PROJECT_SOURCE_DIR}/include/system
	${PROJECT_SOURCE_DIR}/include/views
	${PROJECT_SOURCE_DIR}/include)

add_definitions (-DLOGNAME="${LOGNAME}")

if(EY3_WITH_CV)
	add_definitions (-DEY3_WITH_CV)
endif(EY3_WITH_CV)


# Native app glue (linked in apps)
add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)


# Log
find_library(log-lib log)


# OpenCV (only needed by the computer vision module)
if(EY3_WITH_CV)
	include_directories(SYSTEM ${OPENCV_DIR}/jni/include)

	add_library(cv_core STATIC IMPORTED)
	set_target_properties(cv_core PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_core.a)

	add_library(cv_imgproc STATIC IMPORTED)
	set_target_properties(cv_imgproc PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_imgproc.a)

	add_library(cv_imgcodecs STATIC IMPORTED)
	set_target_properties(cv_imgcodecs PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_imgcodecs.a)


	# OpenCV 3dparties
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
endif(EY3_WITH_CV)


# Link Libraries
target_link_libraries(${LIBNAME}
	android
	${log-lib}
	EGL
	GLESv3
	z
	)

if(EY3_WITH_CV)
	target_link_libraries(${LIBNAME}
		mediandk
		camera2ndk
		cv_imgcodecs
		cv_imgproc
		cv_core
		cv_jpeg-turbo
		cv_openjp2
		cv_png
		cv_tiff
		cv_tbb
		)
endif(EY3_WITH_CV)


# CREATE KEYSTORE
# Generated once at the top of the build tree (CMAKE_BINARY_DIR) so every app
# subdirectory can reference it via the same absolute path when signing.
# Wrapped in add_custom_target(): a standalone add_custom_command() with no
# target consuming it is pruned by the Ninja generator, so a DEPENDS on its
# OUTPUT from another directory silently resolves to "no known rule".
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore
	COMMAND $ENV{JAVA_HOME}/bin/keytool -genkeypair -validity 10000 -keystore ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore -alias ${KEYALIAS} -keyalg RSA -keysize 2048 -storepass ${STOREPASS} -keypass ${KEYPASS} -dname "CN=${COMPANY},O=Android,C=ES"
	COMMENT "Creating keystore")
add_custom_target(keystore DEPENDS ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore)


# APPS
add_subdirectory(apps/ey3-triangle)

if(EY3_WITH_CV)
	# All three depend on AssetLoader::loadImageAsset (ey3-maze loads its
	# sprite sheets with it) / Camera / CameraView, which need OpenCV
	add_subdirectory(apps/ey3-background-img)
	add_subdirectory(apps/ey3-maze)
	add_subdirectory(apps/ey3-orb-demo)
endif(EY3_WITH_CV)
	
