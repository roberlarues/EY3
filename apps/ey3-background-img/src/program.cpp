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

	engine.run();
}
