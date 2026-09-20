# ORB feature matching on the live camera, drawn as a full-screen background.
#
# Desktop build. ey3 has to be installed first -- run scripts/install-sdk.sh
# in the EY3 repository -- and this example is built like any other project
# using it, from its own directory:
#
#   cmake -S . -B build -G Ninja      # -DCMAKE_PREFIX_PATH=... if not in /usr/local
#   cmake --build build
#   ./build/ey3-orb-demo
#
# The Android build lives next door in android/. See README.md.
cmake_minimum_required(VERSION 3.16)
project(ey3-orb-demo CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ey3 REQUIRED)

# The app's own sources, listed once for both platforms.
include(${CMAKE_CURRENT_SOURCE_DIR}/app.cmake)

add_executable(ey3-orb-demo desktop/main_desktop.cpp ${APP_SOURCES})
target_include_directories(ey3-orb-demo PRIVATE include)
target_link_libraries(ey3-orb-demo PRIVATE ey3::ey3)
