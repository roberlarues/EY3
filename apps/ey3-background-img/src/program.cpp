#include "program.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace ey3;

void runProgram(Engine& engine) {
	AssetLoader assetLoader;
	cv::Mat image = assetLoader.loadImageAsset("images/example.png");
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

	Background bg(image.ptr(), image.cols, image.rows);
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
