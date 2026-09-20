# An image from assets/ stretched across the screen.
#
# Desktop build. ey3 has to be installed first -- run scripts/install-sdk.sh
# in the EY3 repository -- and this example is built like any other project
# using it, from its own directory:
#
#   cmake -S . -B build -G Ninja      # -DCMAKE_PREFIX_PATH=... if not in /usr/local
#   cmake --build build
#   ./build/ey3-background-img
#
# The Android build lives next door in android/. See README.md.
cmake_minimum_required(VERSION 3.16)
project(ey3-background-img CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ey3 REQUIRED)

# The app's own sources, listed once for both platforms.
include(${CMAKE_CURRENT_SOURCE_DIR}/app.cmake)

add_executable(ey3-background-img desktop/main_desktop.cpp ${APP_SOURCES})
target_include_directories(ey3-background-img PRIVATE include)
target_link_libraries(ey3-background-img PRIVATE ey3::ey3)

# AssetLoader reads from an 'assets' folder next to the executable; the
# Android build packages that same directory into the APK, so both platforms
# see the same paths.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
	add_custom_command(TARGET ey3-background-img POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink
			${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ey3-background-img>/assets
		COMMENT "Linking assets")
endif()
