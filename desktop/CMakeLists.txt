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
# OpenCV 5 renamed the features2d/calib3d modules to features/calib; the
# opencv2/features2d.hpp, opencv2/calib3d.hpp headers and their API are
# unaffected, only the find_package component/library names changed.
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio features calib flann)

include(${EY3_ROOT}/cmake/sources.cmake)

add_library(ey3 SHARED
	${EY3_SHARED_SOURCES}
	${EY3_ROOT}/src/system/window_desktop.cpp
	${EY3_ROOT}/src/system/asset_loader_desktop.cpp)

target_compile_definitions(ey3 PUBLIC LOGNAME="ey3")
if(EY3_WITH_CV)
	target_compile_definitions(ey3 PUBLIC EY3_WITH_CV)
	target_sources(ey3 PRIVATE ${EY3_ROOT}/src/system/camera_desktop.cpp)
endif(EY3_WITH_CV)

target_include_directories(ey3 PUBLIC
	$<BUILD_INTERFACE:${EY3_ROOT}/include>
	$<BUILD_INTERFACE:${EY3_ROOT}/include/graphics>
	$<BUILD_INTERFACE:${EY3_ROOT}/include/system>
	$<BUILD_INTERFACE:${EY3_ROOT}/include/views>
	$<INSTALL_INTERFACE:include/ey3>
	$<INSTALL_INTERFACE:include/ey3/graphics>
	$<INSTALL_INTERFACE:include/ey3/system>
	$<INSTALL_INTERFACE:include/ey3/views>
	${OpenCV_INCLUDE_DIRS})

target_link_libraries(ey3 PUBLIC glfw EGL GLESv2 ${OpenCV_LIBS})

# --- Installable package: after `cmake --install`, any other project just
# does find_package(ey3) + target_link_libraries(app PRIVATE ey3::ey3), same
# as it already does for glfw3/OpenCV -- no headers/lib copied into it.
# See ../scripts/install-sdk.sh and ../templates/ey3-starter.
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ey3 EXPORT ey3Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${EY3_ROOT}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ey3)
install(EXPORT ey3Targets FILE ey3Targets.cmake NAMESPACE ey3::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ey3)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ey3Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/ey3Config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ey3)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/ey3ConfigVersion.cmake
	VERSION 0.1.0
	COMPATIBILITY AnyNewerVersion)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/ey3Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/ey3ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ey3)

# Each app gets its own output directory (rather than sharing this project's
# single CMAKE_CURRENT_BINARY_DIR) so their same-named 'assets' symlinks,
# each resolved relative to that app's own executable, don't collide.
# assets_dir is optional -- apps with no assets/ folder (e.g. ey3-orb-demo,
# which only uses the built-in default shader) can omit it.
function(ey3_add_desktop_app target)
	set(assets_dir ${ARGN})
	set_target_properties(${target} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${target})
	target_link_libraries(${target} PRIVATE ey3)
	if(assets_dir)
		add_custom_command(TARGET ${target} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${assets_dir} $<TARGET_FILE_DIR:${target}>/assets
			COMMENT "Linking assets for ${target}")
	endif()
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

# ey3-orb-demo-desktop
if(EY3_WITH_CV)
	add_executable(ey3-orb-demo-desktop
		${EY3_ROOT}/apps/ey3-orb-demo/src/main_desktop.cpp
		${EY3_ROOT}/apps/ey3-orb-demo/src/program.cpp
		${EY3_ROOT}/apps/ey3-orb-demo/src/orbTest.cpp)
	target_include_directories(ey3-orb-demo-desktop PRIVATE ${EY3_ROOT}/apps/ey3-orb-demo/include)
	ey3_add_desktop_app(ey3-orb-demo-desktop)
endif(EY3_WITH_CV)
