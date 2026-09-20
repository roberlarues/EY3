#include "camera_desktop.h"

#include "app_cmd.h"
#include "log.h"

using namespace ey3;
using namespace cv;

Camera::Camera(int32_t deviceIndex): deviceIndex(deviceIndex) {
	LOGI("Creating camera");
}

Camera::~Camera() {
	LOGI("Deleting camera");
	close();
}

bool Camera::hasPermission() {
	return true;
}

void Camera::requestPermission() {
}

bool Camera::open(CameraFacing cameraFacing, int32_t cameraWidth, int32_t cameraHeight) {
	if (opened) {
		LOGE("This camera is already open!");
		return false;
	}

	LOGI("Opening camera");

	this->cameraFacing = cameraFacing;
	this->cameraWidth = cameraWidth;
	this->cameraHeight = cameraHeight;

	if (!capture.isOpened() && !capture.open(deviceIndex)) {
		LOGE("Failed to open webcam %d", deviceIndex);
		return false;
	}

	capture.set(CAP_PROP_FRAME_WIDTH, cameraWidth);
	capture.set(CAP_PROP_FRAME_HEIGHT, cameraHeight);

	frameWidth = (int32_t) capture.get(CAP_PROP_FRAME_WIDTH);
	frameHeight = (int32_t) capture.get(CAP_PROP_FRAME_HEIGHT);
	LOGI("Camera opened: %dx%d", frameWidth, frameHeight);

	loaded = true;
	opened = true;
	return true;
}

void Camera::close() {
	LOGI("Closing camera");
	opened = false;

	if (capture.isOpened()) {
		capture.release();
	}
}

bool Camera::getFrame(OutputArray out) {
	if (!opened) {
		return false;
	}

	Mat frame;
	if (!capture.read(frame) || frame.empty()) {
		return false;
	}

	cvtColor(frame, out, COLOR_BGR2RGB);
	return true;
}

void Camera::handleCmd(int32_t cmd, IWindow* window) {
	switch (cmd) {
		case APP_CMD_INIT_WINDOW:
			if (loaded && !opened) {
				open(cameraFacing, cameraWidth, cameraHeight);
			}
			break;
		case APP_CMD_TERM_WINDOW:
			close();
			break;
	}
}

bool Camera::isLoaded() {
	return loaded;
}

bool Camera::isOpened() {
	return opened;
}

int32_t Camera::getFrameWidth() {
	return frameWidth;
}

int32_t Camera::getFrameHeight() {
	return frameHeight;
}

float Camera::getFieldOfView() {
	return 0.0f;   // unknown: nothing in V4L2 reports the optics
}

int32_t Camera::getImageRotation() {
	return 0;
}
