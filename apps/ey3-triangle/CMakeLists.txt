project(EY3-Triangle)

set(LIBNAME "ey3-triangle")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib/${ANDROID_ABI})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib/${ANDROID_ABI})
	
# Android main lib
add_library(${LIBNAME} SHARED src/main.cpp src/program.cpp src/triangle.cpp)
target_include_directories(${LIBNAME} PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue include)

# Link Libraries
target_link_libraries(${LIBNAME}
	android
	native_app_glue
	ey3)


# CUSTOM TARGETS
add_custom_target(${LIBNAME}-run 
	COMMAND $ENV{ANDROID_HOME}/platform-tools/adb install -r ${LIBNAME}.apk 
	DEPENDS ${LIBNAME}.apk
	COMMENT "Launching ${LIBNAME} APK")
add_custom_target(${LIBNAME}-apk DEPENDS ${LIBNAME}.apk COMMENT "Creating release APK.")
add_custom_target(${LIBNAME}-unsigned DEPENDS ${LIBNAME}.unsigned.apk COMMENT "Creating unsigned APK.")

#CUSTOM COMMANDS
add_custom_command(OUTPUT ${LIBNAME}.unsigned.apk
	COMMAND ${CMAKE_COMMAND} -E remove ${LIBNAME}.unsigned.apk
	COMMAND ${BUILD_TOOLS}/aapt package -f -M ${PROJECT_SOURCE_DIR}/AndroidManifest.xml --auto-add-overlay -S ${PROJECT_SOURCE_DIR}/res -S ${PROJECT_SOURCE_DIR}/../../res -A ${PROJECT_SOURCE_DIR}/assets -I $ENV{ANDROID_HOME}/platforms/${ANDROID_PLATFORM}/android.jar -F ${LIBNAME}.unsigned.apk .
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
