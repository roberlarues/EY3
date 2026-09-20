# Turns a directory of sources into an installable Android APK, with no Java
# and no Gradle: aapt packages it, jarsigner signs it and zipalign finishes
# it. One copy of the recipe, used by this repository's examples, by
# templates/ey3-starter and by anything built against the installed SDK.
#
# Expects the engine targets (`ey3`, `native_app_glue`) to exist already --
# include ey3-android-engine.cmake first -- plus the usual signing variables
# (KEYSTORE, KEYALIAS, STOREPASS, KEYPASS, COMPANY), which default below.
#
# Use:
#   ey3_add_android_app(my-app
#       SOURCES src/main_android.cpp ${APP_ROOT}/src/program.cpp
#       INCLUDES include
#       LIBRARIES cv_features2d)     # only if the app needs more than ey3
#
# The directory this is called from must hold AndroidManifest.xml and res/;
# an assets/ folder next to them is packaged when it is there, or pass
# ASSETS <dir> for one kept elsewhere (a project sharing assets between
# desktop and Android keeps them at its root, and passes that path). A path
# that does not exist yet is simply ignored.
#
# Targets created, for an app called my-app:
#   my-app-unsigned   the APK, no keystore needed
#   my-app-apk        signed and aligned
#   my-app-run        installs it with adb

foreach(var KEYSTORE:release KEYALIAS:alias STOREPASS:changeme KEYPASS:changeme COMPANY:company)
	string(REPLACE ":" ";" pair ${var})
	list(GET pair 0 name)
	list(GET pair 1 fallback)
	if(NOT DEFINED ${name})
		set(${name} ${fallback})
	endif()
endforeach()

# The command line tools, resolved once and cached. Spelling them out as
# "$ENV{ANDROID_HOME}/build-tools/.../aapt" means CMake expands them while
# configuring, so configuring from a shell without those variables bakes
# broken paths into the rules and the failure only shows up later, when
# building. This fails now, with a message that says what to do.
foreach(spec "EY3_AAPT;aapt;$ENV{ANDROID_HOME}/build-tools/${SDK_VERSION}"
             "EY3_ZIPALIGN;zipalign;$ENV{ANDROID_HOME}/build-tools/${SDK_VERSION}"
             "EY3_KEYTOOL;keytool;$ENV{JAVA_HOME}/bin"
             "EY3_JARSIGNER;jarsigner;$ENV{JAVA_HOME}/bin")
	list(GET spec 0 tool_var)
	list(GET spec 1 tool_name)
	list(GET spec 2 tool_hint)
	find_program(${tool_var} NAMES ${tool_name} HINTS ${tool_hint})
	if(NOT ${tool_var})
		message(FATAL_ERROR "Could not find '${tool_name}' (looked in ${tool_hint} and "
			"the PATH). Set ANDROID_HOME / JAVA_HOME and SDK_VERSION before configuring "
			"-- see doc/environment-setup.md.")
	endif()
	message(STATUS "Using ${tool_name}: ${${tool_var}}")
endforeach()

set(EY3_ANDROID_JAR "$ENV{ANDROID_HOME}/platforms/${ANDROID_PLATFORM}/android.jar")
if(NOT EXISTS ${EY3_ANDROID_JAR})
	message(FATAL_ERROR "No android.jar at ${EY3_ANDROID_JAR}. Check ANDROID_HOME and "
		"ANDROID_PLATFORM (now '${ANDROID_PLATFORM}') -- see doc/environment-setup.md.")
endif()

# One keystore for every app, at the top of the build tree. The script behind
# it does nothing when the file is already there: keytool refuses to add an
# alias twice, so a rule that just ran it again would stop the build.
if(NOT TARGET keystore)
	set(EY3_KEYSTORE_FILE ${CMAKE_BINARY_DIR}/${KEYSTORE}.keystore)
	add_custom_command(OUTPUT ${EY3_KEYSTORE_FILE}
		COMMAND ${CMAKE_COMMAND}
			-DKEYSTORE_FILE=${EY3_KEYSTORE_FILE}
			-DKEYTOOL=${EY3_KEYTOOL} -DKEYALIAS=${KEYALIAS}
			-DSTOREPASS=${STOREPASS} -DKEYPASS=${KEYPASS} -DCOMPANY=${COMPANY}
			-P ${CMAKE_CURRENT_LIST_DIR}/create-keystore.cmake
		COMMENT "Creating keystore")
	add_custom_target(keystore DEPENDS ${EY3_KEYSTORE_FILE})
endif()

function(ey3_add_android_app name)
	cmake_parse_arguments(APP "" "ASSETS" "SOURCES;INCLUDES;LIBRARIES" ${ARGN})
	if(NOT APP_SOURCES)
		message(FATAL_ERROR "ey3_add_android_app(${name}): no SOURCES given")
	endif()

	add_library(${name} SHARED ${APP_SOURCES})
	target_include_directories(${name} PRIVATE ${APP_INCLUDES})
	target_link_libraries(${name} android native_app_glue ey3 ${APP_LIBRARIES})

	set_target_properties(${name} PROPERTIES
		# ANativeActivity_onCreate is only reached from the activity glue, so
		# the linker has to be told to keep it.
		LINK_FLAGS "-u ANativeActivity_onCreate"
		# The .so is linked straight into apk-root/, the tree aapt packs
		# below, so that tree holds the app's native libs and nothing else.
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apk-root/lib/${ANDROID_ABI})

	# The NDK toolchain adds -g even in Release, and nothing here strips it
	# before packaging: debug sections alone are most of an unstripped APK.
	add_custom_command(TARGET ${name} POST_BUILD
		COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${name}>
		COMMENT "Stripping debug info from ${name}")

	set(resources -S ${CMAKE_CURRENT_SOURCE_DIR}/res)
	if(NOT APP_ASSETS)
		set(APP_ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
	endif()
	set(assets "")
	if(EXISTS ${APP_ASSETS})
		set(assets -A ${APP_ASSETS})
	endif()

	# aapt copies the *contents* of the directory given as its last argument
	# into the APK, keeping their relative paths, which is how lib/<abi>/*.so
	# gets in. It must be apk-root and not "." (the build directory itself):
	# "." would also sweep in CMake's object files and, on a rebuild, the
	# APKs the previous run left behind.
	add_custom_command(OUTPUT ${name}.unsigned.apk
		COMMAND ${EY3_AAPT} package -f -M ${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml
			--auto-add-overlay ${resources} ${assets}
			-I ${EY3_ANDROID_JAR} -F ${name}.unsigned.apk apk-root
		DEPENDS ${name}
		COMMENT "Building ${name} apk")

	add_custom_command(OUTPUT ${name}.signed.apk
		COMMAND ${EY3_JARSIGNER} -keystore ${EY3_KEYSTORE_FILE}
			-sigalg SHA256withRSA -digestalg SHA-256
			-storepass ${STOREPASS} -keypass ${KEYPASS}
			-signedjar ${name}.signed.apk ${name}.unsigned.apk ${KEYALIAS}
		DEPENDS ${name}.unsigned.apk ${EY3_KEYSTORE_FILE}
		COMMENT "Signing ${name}")

	add_custom_command(OUTPUT ${name}.apk
		COMMAND ${EY3_ZIPALIGN} -f 4 ${name}.signed.apk ${name}.apk
		DEPENDS ${name}.signed.apk
		COMMENT "Aligning ${name}")

	add_custom_target(${name}-unsigned DEPENDS ${name}.unsigned.apk)
	add_custom_target(${name}-apk DEPENDS ${name}.apk)
	add_custom_target(${name}-run
		COMMAND $ENV{ANDROID_HOME}/platform-tools/adb install -r ${name}.apk
		DEPENDS ${name}.apk
		COMMENT "Installing ${name}")
endfunction()
