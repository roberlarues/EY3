#include "program.h"
#include "orbTest.h"

using namespace cv;
using namespace ey3;

void runProgram(Engine& engine) {
	Camera camera;
	engine.getCmdHandler()->addListener(&camera);

	CameraView cameraView(640 * 2, 480);
	engine.getRenderer()->addRenderizable(&cameraView);

	OrbTest orbTest(640, 480);
	engine.getInputHandler()->addListener(&orbTest);

	Mat frame;
	while (!engine.hasTerminated()) {
		engine.pollEvents();

		if (engine.isInForeground()) {
			// On Android, android.permission.CAMERA is granted asynchronously by the
			// user via the system dialog; retry until isOpened() succeeds. open() is
			// a cheap no-op once already open, or while waiting on the pending
			// permission. On desktop this just opens the webcam on the first frame.
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
}
