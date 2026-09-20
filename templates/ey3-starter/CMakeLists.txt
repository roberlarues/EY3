cmake_minimum_required(VERSION 3.16)
project(ey3-starter CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ey3 must be installed first (see ../scripts/install-sdk.sh in the main EY3
# repo, or README.md here). If it isn't on CMake's default search path,
# point at it with -DCMAKE_PREFIX_PATH=/path/to/prefix.
find_package(ey3 REQUIRED)

# The app's own sources, listed once for both platforms.
include(${CMAKE_CURRENT_SOURCE_DIR}/app.cmake)

add_executable(app desktop/main_desktop.cpp ${APP_SOURCES})
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE ey3::ey3)

# AssetLoader reads from an 'assets' folder next to the executable. The
# Android build packages the same directory into the APK, so both platforms
# see the same paths. Nothing to do until the project has one.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
	add_custom_command(TARGET app POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink
			${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:app>/assets
		COMMENT "Linking assets")
endif()
