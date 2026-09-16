# Source files shared by every platform. EY3_ROOT and EY3_WITH_CV must be set
# before including this file. Each platform's CMakeLists.txt appends its own
# platform glue (window/asset-loader implementation) to EY3_SHARED_SOURCES.

set(EY3_SHARED_SOURCES
	${EY3_ROOT}/src/graphics/renderer.cpp
	${EY3_ROOT}/src/graphics/shader.cpp
	${EY3_ROOT}/src/graphics/texture.cpp
	${EY3_ROOT}/src/system/engine.cpp
	${EY3_ROOT}/src/system/cmd_handler.cpp
	${EY3_ROOT}/src/system/input_handler.cpp
	${EY3_ROOT}/src/views/background.cpp)

if(EY3_WITH_CV)
	list(APPEND EY3_SHARED_SOURCES ${EY3_ROOT}/src/views/camera_view.cpp)
endif(EY3_WITH_CV)
