#include <string>
#include <android_native_app_glue.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ey3.h>

using namespace ey3;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
	LOGI("BEGIN Android Main");
	Engine engine(app);

	AssetLoader assetLoader;
	cv::Mat image = assetLoader.loadImageAsset(app, "images/example.png");
	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
	Background bg(image.ptr(), image.cols, image.rows);
	engine.getRenderer()->addRenderizable(&bg);
	
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			engine.getRenderer()->renderFrame();
		}
	}

	LOGI("END Android Main");
}

