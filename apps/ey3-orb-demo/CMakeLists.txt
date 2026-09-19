project(EY3-ORB-Demo)

set(LIBNAME "ey3-orb-demo")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
# The .so is linked straight into apk-root/, the directory tree aapt packs
# into the APK further down, so that tree holds the app's native libs and
# nothing else.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY apk-root/lib/${ANDROID_ABI})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY apk-root/lib/${ANDROID_ABI})
	
# Android main lib
add_library(${LIBNAME} SHARED src/main.cpp src/program.cpp src/orbTest.cpp)
target_include_directories(${LIBNAME} PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue include)
ey3_strip_target(${LIBNAME})

# OpenCV
include_directories(SYSTEM ${OPENCV_DIR}/jni/include)

add_library(cv_features2d STATIC IMPORTED)
set_target_properties(cv_features2d PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_features2d.a)

add_library(cv_flann STATIC IMPORTED)
set_target_properties(cv_flann PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_flann.a)

add_library(cv_calib3d STATIC IMPORTED)
set_target_properties(cv_calib3d PROPERTIES IMPORTED_LOCATION ${OPENCV_DIR}/staticlibs/${ANDROID_ABI}/libopencv_calib3d.a)

# Link Libraries
target_link_libraries(${LIBNAME}
	android
	native_app_glue
	ey3
	cv_features2d
	cv_calib3d
	cv_flann)

# CUSTOM TARGETS
add_custom_target(${LIBNAME}-run 
	COMMAND $ENV{ANDROID_HOME}/platform-tools/adb install -r ${LIBNAME}.apk 
	DEPENDS ${LIBNAME}.apk
	COMMENT "Launching ${LIBNAME} APK")
add_custom_target(${LIBNAME}-apk DEPENDS ${LIBNAME}.apk COMMENT "Creating release APK.")
add_custom_target(${LIBNAME}-unsigned DEPENDS ${LIBNAME}.unsigned.apk COMMENT "Creating unsigned APK.")


#CUSTOM COMMANDS
# aapt copies the *contents* of the directory given as its last argument
# into the APK, keeping their relative paths, which is how lib/<abi>/*.so
# gets in. That argument must be apk-root and not "." (the build directory
# itself): "." would also sweep in CMake's object files and, on a rebuild,
# the APKs the previous run left behind.
add_custom_command(OUTPUT ${LIBNAME}.unsigned.apk
	COMMAND ${BUILD_TOOLS}/aapt package -f -M ${PROJECT_SOURCE_DIR}/AndroidManifest.xml --auto-add-overlay -S ${PROJECT_SOURCE_DIR}/res -S ${PROJECT_SOURCE_DIR}/../../res -I $ENV{ANDROID_HOME}/platforms/${ANDROID_PLATFORM}/android.jar -F ${LIBNAME}.unsigned.apk apk-root
	DEPENDS ${LIBNAME}
	COMMENT "Building apk")

add_custom_command(OUTPUT ${LIBNAME}.signed.apk
	COMMAND jarsigner -keystore ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore -sigalg SHA256withRSA -digestalg SHA-256 -storepass ${STOREPASS} -keypass ${KEYPASS} -signedjar ${LIBNAME}.signed.apk ${LIBNAME}.unsigned.apk ${KEYALIAS}
	DEPENDS ${LIBNAME}.unsigned.apk ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore
	COMMENT "Signing APK")
	
add_custom_command(OUTPUT ${LIBNAME}.apk 
	COMMAND ${BUILD_TOOLS}/zipalign -f 4 ${LIBNAME}.signed.apk ${LIBNAME}.apk
	DEPENDS ${LIBNAME}.signed.apk
	COMMENT "Calling zipalign")
