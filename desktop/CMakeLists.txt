# Desktop simulator build for EY3 (Linux).
#
# Compiles the same shared engine sources (cmake/sources.cmake) as the
# Android build, swapping only the Android-specific platform glue
# (window/asset-loader) for GLFW/filesystem equivalents. See
# doc/environment-setup.md and README.md for the Android build.
#
# Usage:
#   cmake -S desktop -B build-desktop -G Ninja
#   cmake --build build-desktop
#   ./build-desktop/ey3-triangle-desktop/ey3-triangle-desktop
#   ./build-desktop/ey3-background-img-desktop/ey3-background-img-desktop
cmake_minimum_required(VERSION 3.16)
project(EY3Desktop CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(EY3_WITH_CV "Enable the OpenCV integration (asset image loading, CameraView)" ON)

set(EY3_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(glfw3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

include(${EY3_ROOT}/cmake/sources.cmake)

add_library(ey3 STATIC
	${EY3_SHARED_SOURCES}
	${EY3_ROOT}/src/system/window_desktop.cpp
	${EY3_ROOT}/src/system/asset_loader_desktop.cpp)

target_compile_definitions(ey3 PUBLIC LOGNAME="ey3")
if(EY3_WITH_CV)
	target_compile_definitions(ey3 PUBLIC EY3_WITH_CV)
endif(EY3_WITH_CV)

target_include_directories(ey3 PUBLIC
	${EY3_ROOT}/include
	${EY3_ROOT}/include/graphics
	${EY3_ROOT}/include/system
	${EY3_ROOT}/include/views
	${OpenCV_INCLUDE_DIRS})

target_link_libraries(ey3 PUBLIC glfw EGL GLESv2 ${OpenCV_LIBS})

# Each app gets its own output directory (rather than sharing this project's
# single CMAKE_CURRENT_BINARY_DIR) so their same-named 'assets' symlinks,
# each resolved relative to that app's own executable, don't collide.
function(ey3_add_desktop_app target assets_dir)
	set_target_properties(${target} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${target})
	target_link_libraries(${target} PRIVATE ey3)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${assets_dir} $<TARGET_FILE_DIR:${target}>/assets
		COMMENT "Linking assets for ${target}")
endfunction()

# ey3-triangle-desktop
add_executable(ey3-triangle-desktop
	${EY3_ROOT}/apps/ey3-triangle/src/main_desktop.cpp
	${EY3_ROOT}/apps/ey3-triangle/src/program.cpp
	${EY3_ROOT}/apps/ey3-triangle/src/triangle.cpp)
target_include_directories(ey3-triangle-desktop PRIVATE ${EY3_ROOT}/apps/ey3-triangle/include)
ey3_add_desktop_app(ey3-triangle-desktop ${EY3_ROOT}/apps/ey3-triangle/assets)

# ey3-background-img-desktop
add_executable(ey3-background-img-desktop
	${EY3_ROOT}/apps/ey3-background-img/src/main_desktop.cpp
	${EY3_ROOT}/apps/ey3-background-img/src/program.cpp)
target_include_directories(ey3-background-img-desktop PRIVATE ${EY3_ROOT}/apps/ey3-background-img/include)
ey3_add_desktop_app(ey3-background-img-desktop ${EY3_ROOT}/apps/ey3-background-img/assets)
