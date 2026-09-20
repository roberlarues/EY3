#include "head_tracker.h"

#include <math.h>

#include <chrono>

#include <opencv2/imgproc.hpp>

#include "log.h"

using namespace ey3;

// Faces are looked for on a frame shrunk to this width. YuNet copes fine at
// this size and the work drops by several times, which matters: every
// millisecond here is lag between moving your head and seeing the scene move.
static const int DETECT_WIDTH = 320;

// How sure the detector has to be. Lower than OpenCV's default, because a
// face lit from a screen and seen from below is not a portrait photo.
static const float SCORE_THRESHOLD = 0.7f;
static const float NMS_THRESHOLD = 0.3f;
static const int TOP_K = 50;

// One Euro filter: the cutoff at rest, and how fast it opens up with speed.
static const float MIN_CUTOFF = 1.0f;
static const float BETA = 0.4f;
static const float SPEED_CUTOFF = 1.0f;

// After this long without a face, the viewpoint drifts back to its resting
// place instead of staying frozen wherever the player was last seen.
static const float LOST_FACE_SECONDS = 0.5f;
static const float RETURN_SPEED = 1.5f;

static float alphaFor(float cutoff, float deltaTime) {
	float tau = 1.0f / (2.0f * (float) M_PI * cutoff);
	return 1.0f / (1.0f + tau / deltaTime);
}

float HeadTracker::Axis::filter(float sample, float deltaTime, float minCutoff, float beta) {
	if (!started) {
		started = true;
		value = sample;
		speed = 0.0f;
		return value;
	}

	float rawSpeed = (sample - value) / deltaTime;
	speed += alphaFor(SPEED_CUTOFF, deltaTime) * (rawSpeed - speed);

	float cutoff = minCutoff + beta * fabsf(speed);
	value += alphaFor(cutoff, deltaTime) * (sample - value);
	return value;
}

HeadTracker::HeadTracker()
	: eyePosition{0.0f, 0.0f, 50.0f}, restingPosition{0.0f, 0.0f, 50.0f},
	  faceFound(false), timeWithoutFace(0.0f), fieldOfViewDegrees(60.0f),
	  cameraOffsetX(0.0f), cameraOffsetY(0.0f), eyeDistanceCm(6.3f), mirrored(true),
	  detectionMillis(0.0f) {
	axisX.started = axisY.started = axisZ.started = false;
	detection.score = 0.0f;
}

bool HeadTracker::init(AssetLoader* assetLoader, const char* modelAsset) {
	const std::string model = assetLoader->loadStringAsset(modelAsset);
	if (model.empty()) {
		LOGE("Face model not found: %s -- head tracking is off", modelAsset);
		return false;
	}

	// The model goes in straight from the asset: no temporary file, which
	// there is nowhere sensible to put on Android anyway.
	const std::vector<uchar> buffer(model.begin(), model.end());
	detector = cv::FaceDetectorYN::create("onnx", buffer, std::vector<uchar>(),
	                                      cv::Size(DETECT_WIDTH, DETECT_WIDTH),
	                                      SCORE_THRESHOLD, NMS_THRESHOLD, TOP_K);
	if (detector.empty()) {
		LOGE("The face detector would not load %s", modelAsset);
		return false;
	}

	LOGI("Face detector ready (%s, %d KB)", modelAsset, (int) (model.size() / 1024));
	return true;
}

void HeadTracker::setFieldOfView(float degrees) {
	fieldOfViewDegrees = degrees;
}

void HeadTracker::setCameraOffset(float xCm, float yCm) {
	cameraOffsetX = xCm;
	cameraOffsetY = yCm;
}

void HeadTracker::setEyeDistance(float centimetres) {
	eyeDistanceCm = centimetres;
}

void HeadTracker::setMirrored(bool mirrored) {
	this->mirrored = mirrored;
}

void HeadTracker::setRestingPosition(const Vec3& position) {
	restingPosition = position;
}

bool HeadTracker::findFace(const cv::Mat& frame) {
	if (detector.empty()) {
		return false;
	}

	float scale = (float) DETECT_WIDTH / frame.cols;
	cv::Mat small;
	cv::resize(frame, small, cv::Size(DETECT_WIDTH, (int) (frame.rows * scale)), 0, 0, cv::INTER_AREA);

	// The model was trained on BGR, OpenCV's own order, and the camera hands
	// over RGB. Feeding it the wrong way round costs detections.
	cv::Mat bgr;
	cv::cvtColor(small, bgr, cv::COLOR_RGB2BGR);

	cv::Mat faces;
	detector->setInputSize(bgr.size());
	detector->detect(bgr, faces);
	if (faces.rows < 1) {
		return false;
	}

	// Rows come out sorted by score; the first is the best. Each is
	// [x, y, w, h, rightEye, leftEye, nose, mouth corners, score].
	const float* row = faces.ptr<float>(0);
	detection.box = cv::Rect((int) (row[0] / scale), (int) (row[1] / scale),
	                         (int) (row[2] / scale), (int) (row[3] / scale));
	detection.rightEye = cv::Point2f(row[4] / scale, row[5] / scale);
	detection.leftEye = cv::Point2f(row[6] / scale, row[7] / scale);
	detection.score = row[14];
	return true;
}

void HeadTracker::update(const cv::Mat& frame, float deltaTime) {
	if (frame.empty() || deltaTime <= 0.0f) {
		return;
	}

	std::chrono::steady_clock::time_point before = std::chrono::steady_clock::now();
	faceFound = findFace(frame);
	detectionMillis = std::chrono::duration<float, std::milli>(
		std::chrono::steady_clock::now() - before).count();

	if (!faceFound) {
		detection.score = 0.0f;
		timeWithoutFace += deltaTime;
		if (timeWithoutFace > LOST_FACE_SECONDS) {
			// Drift back to the resting viewpoint rather than freeze.
			float t = fminf(1.0f, RETURN_SPEED * deltaTime);
			eyePosition.x += (restingPosition.x - eyePosition.x) * t;
			eyePosition.y += (restingPosition.y - eyePosition.y) * t;
			eyePosition.z += (restingPosition.z - eyePosition.z) * t;
			axisX.started = axisY.started = axisZ.started = false;
		}
		return;
	}
	timeWithoutFace = 0.0f;

	// Pinhole camera: eyes a known distance apart, so many pixels apart, are
	// this far away; and their offset from the centre of the image is this
	// angle. The field of view is the one across the longer side, because
	// Android turns frames upright for a portrait screen.
	float longSide = (float) (frame.cols > frame.rows ? frame.cols : frame.rows);
	float focalPixels = (longSide / 2.0f)
		/ tanf(fieldOfViewDegrees * 0.5f * (float) M_PI / 180.0f);

	float dx = detection.leftEye.x - detection.rightEye.x;
	float dy = detection.leftEye.y - detection.rightEye.y;
	float eyeDistancePixels = sqrtf(dx * dx + dy * dy);
	if (eyeDistancePixels < 1.0f) {
		return;
	}

	float z = focalPixels * eyeDistanceCm / eyeDistancePixels;
	float midX = (detection.leftEye.x + detection.rightEye.x) / 2.0f;
	float midY = (detection.leftEye.y + detection.rightEye.y) / 2.0f;

	float x = (midX - frame.cols * 0.5f) * z / focalPixels;
	float y = -(midY - frame.rows * 0.5f) * z / focalPixels;   // image y grows down
	if (mirrored) {
		x = -x;
	}

	// From the lens to the centre of the screen.
	x += cameraOffsetX;
	y += cameraOffsetY;

	eyePosition.x = axisX.filter(x, deltaTime, MIN_CUTOFF, BETA);
	eyePosition.y = axisY.filter(y, deltaTime, MIN_CUTOFF, BETA);
	eyePosition.z = axisZ.filter(z, deltaTime, MIN_CUTOFF, BETA);
}

Vec3 HeadTracker::getEyePosition() const {
	return eyePosition;
}

bool HeadTracker::hasFace() const {
	return faceFound;
}

const FaceDetection& HeadTracker::getDetection() const {
	return detection;
}

float HeadTracker::getDetectionMillis() const {
	return detectionMillis;
}
