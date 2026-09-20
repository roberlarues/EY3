# Builds the ey3 engine for Android, from wherever its sources happen to be.
#
# Two projects need this and must not drift apart: this repository, where the
# sources are the real ones, and any project built against the installed SDK
# (see scripts/install-sdk.sh), where they are a vendored copy. Both include
# this same file; only EY3_SOURCE_ROOT differs.
#
# Expects, before including:
#   EY3_SOURCE_ROOT  the directory holding include/ and src/
#   OPENCV_DIR       an OpenCV Android SDK's sdk/native, with objdetect and dnn
#   LOGNAME          the tag ey3's LOGI/LOGW/LOGE write under
#
# Leaves behind the targets `ey3` and `native_app_glue`.

if(NOT DEFINED EY3_SOURCE_ROOT)
	message(FATAL_ERROR "EY3_SOURCE_ROOT is not set")
endif()
if(NOT DEFINED OPENCV_DIR OR NOT EXISTS ${OPENCV_DIR})
	message(FATAL_ERROR
		"OPENCV_DIR (${OPENCV_DIR}) is not an OpenCV Android SDK's sdk/native. "
		"It must be a build with objdetect and dnn -- see doc/environment-setup.md.")
endif()

set(EY3_ROOT ${EY3_SOURCE_ROOT})
include(${CMAKE_CURRENT_LIST_DIR}/sources.cmake)

add_library(ey3 STATIC
	${EY3_SHARED_SOURCES}
	${EY3_SOURCE_ROOT}/src/system/window_android.cpp
	${EY3_SOURCE_ROOT}/src/system/asset_loader_android.cpp
	${EY3_SOURCE_ROOT}/src/system/camera_android.cpp
	${EY3_SOURCE_ROOT}/src/system/platform_context.cpp)

target_include_directories(ey3 PUBLIC
	${ANDROID_NDK}/sources/android/native_app_glue
	${EY3_SOURCE_ROOT}/include
	${EY3_SOURCE_ROOT}/include/graphics
	${EY3_SOURCE_ROOT}/include/system
	${EY3_SOURCE_ROOT}/include/views)
target_compile_definitions(ey3 PUBLIC LOGNAME="${LOGNAME}")

# The activity glue, which every app links as well.
add_library(native_app_glue STATIC
	${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# OpenCV, as static libraries out of its Android SDK.
include_directories(SYSTEM ${OPENCV_DIR}/jni/include)

# objdetect holds FaceDetectorYN and dnn runs its model, which is what
# HeadTracker needs; protobuf is what dnn reads the model with.
foreach(module core imgproc imgcodecs objdetect dnn)
	add_library(cv_${module} STATIC IMPORTED)
	set_target_properties(cv_${module} PROPERTIES
		IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_${module}.a)
endforeach()

foreach(extra libjpeg-turbo libopenjp2 libpng libtiff libprotobuf tbb)
	add_library(cv_${extra} STATIC IMPORTED)
	set_target_properties(cv_${extra} PROPERTIES
		IMPORTED_LOCATION ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/lib${extra}.a)
	list(APPEND EY3_CV_3RDPARTY cv_${extra})
endforeach()

# The stock OpenCV Android SDK builds core against a few more helpers (NEON
# kernels, Intel's instrumentation stubs, webp, OpenEXR) that a trimmed custom
# build does not have. Link them when they are there.
foreach(extra tegra_hal ittnotify libwebp IlmImf)
	set(extra_path ${OPENCV_DIR}/3rdparty/libs/${ANDROID_ABI}/lib${extra}.a)
	if(EXISTS ${extra_path})
		add_library(cv_${extra} STATIC IMPORTED)
		set_target_properties(cv_${extra} PROPERTIES IMPORTED_LOCATION ${extra_path})
		list(APPEND EY3_CV_3RDPARTY cv_${extra})
	endif()
endforeach()

# Modules the engine itself does not use, imported so that an app can ask for
# them: ey3-orb-demo needs features2d and calib3d for ORB and homographies.
foreach(module features2d calib3d flann)
	set(module_path ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_${module}.a)
	if(EXISTS ${module_path})
		add_library(cv_${module} STATIC IMPORTED)
		set_target_properties(cv_${module} PROPERTIES IMPORTED_LOCATION ${module_path})
	endif()
endforeach()

find_library(EY3_LOG_LIB log)
target_link_libraries(ey3
	android ${EY3_LOG_LIB} EGL GLESv3 z
	mediandk camera2ndk
	cv_objdetect cv_dnn cv_imgcodecs cv_imgproc cv_core
	${EY3_CV_3RDPARTY})
