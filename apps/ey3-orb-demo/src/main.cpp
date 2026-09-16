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
	WindowAndroid window(app);
	Engine engine(&window);

	Camera camera(app);
	engine.getCmdHandler()->addListener(&camera);

	CameraView cameraView(640 * 2, 480);
	engine.getRenderer()->addRenderizable(&cameraView);

	OrbTest orbTest(640, 480);
	engine.getInputHandler()->addListener(&orbTest);

	Mat frame;
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			// android.permission.CAMERA is granted asynchronously by the user via
			// the system dialog; retry until isOpened() succeeds. open() is a cheap
			// no-op once already open, or while waiting on the pending permission.
			if (!camera.isOpened()) {
				camera.open(CameraFacing::BACK, 640, 480);
			}

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
