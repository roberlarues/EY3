#include "program.h"

using namespace ey3;

void runProgram(Engine& engine) {
	AssetLoader assetLoader;
	cv::Mat image = assetLoader.loadImageAsset("images/example.png");

	Background bg(image.ptr(), image.cols, image.rows,
	              image.channels() == 4 ? GL_RGBA : GL_RGB);
	engine.getRenderer()->addRenderizable(&bg);

	// The loop belongs to the app, not to the engine: pump the platform's
	// events, then draw, until the window goes away.
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			engine.getRenderer()->renderFrame();
		}
	}
}
