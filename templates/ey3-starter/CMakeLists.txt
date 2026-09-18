cmake_minimum_required(VERSION 3.16)
project(ey3-starter CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ey3 must be installed first (see ../scripts/install-sdk.sh in the main EY3
# repo, or README.md here). If it isn't on CMake's default search path,
# point at it with -DCMAKE_PREFIX_PATH=/path/to/prefix.
find_package(ey3 REQUIRED)

add_executable(app desktop/main_desktop.cpp src/program.cpp)
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE ey3::ey3)
