#include <string>
#include <android_native_app_glue.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ey3.h>
#include "orbTest.h"

using namespace cv;
using namespace ey3;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
	LOGI("BEGIN Android Main");
	Engine engine(app);

	Camera camera(app);
	engine.getCmdHandler()->addListener(&camera);
	camera.open(CameraFacing::BACK, 640, 480);

	CameraView cameraView(640 * 2, 480);
	engine.getRenderer()->addRenderizable(&cameraView);

	OrbTest orbTest(640, 480);
	engine.getInputHandler()->addListener(&orbTest);

	Mat frame;
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			if (camera.isLoaded()) {
				if (camera.getFrame(frame)) {
					frame = orbTest.processFrame(frame);
					cameraView.setFrame(&frame);
				}
			}

			engine.getRenderer()->renderFrame();
		}
	}

	LOGI("END Android Main");
}

