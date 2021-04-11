#include "camera.h"
#include <string>
#include <vector>
#include <chrono>
#include <opencv2/imgproc/imgproc.hpp>
#include "log.h"

#define MAX_BUF_COUNT 4
#define COLOR_FormatUnknown -1
#define COLOR_FormatYUV420_YV12 12
#define COLOR_FormatYUV420_NV21 21

using namespace ey3;
using namespace cv;

/*
 * Callbacks
 */
static void onDeviceDisconnected(void* /* ctx*/, ACameraDevice* device);
static void onDeviceError(void* /* ctx */, ACameraDevice* device, int error);
static void OnSessionClosed(void* /* ctx */, ACameraCaptureSession* session);
static void OnSessionReady(void* /* ctx */, ACameraCaptureSession* session);
static void OnSessionActive(void* /* ctx */, ACameraCaptureSession* session);

static ACameraDevice_stateCallbacks cameraDeviceCallbacks = {
	.context = nullptr,
	.onDisconnected = ::onDeviceDisconnected,
	.onError = ::onDeviceError,
};

static ACameraCaptureSession_stateCallbacks sessionCallbacks = {
	.onActive = OnSessionActive,
	.onReady = OnSessionReady,
	.onClosed = OnSessionClosed,
};

Camera::Camera(android_app* app): app(app) {
	LOGI("Creating camera");
}

Camera::~Camera() {
	LOGI("Deleting camera");
	close();
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

	cameraManager = ACameraManager_create();
    if (!cameraManager) {
		LOGE("Failed to load Camera Manager");
		close();
		return false;
	}


	std::string cameraId = getFacingCameraId(cameraFacing);

	camera_status_t camera_status = ACAMERA_OK;

	camera_status = ACameraManager_openCamera(cameraManager, cameraId.c_str(), &cameraDeviceCallbacks, &cameraDevice);
	if (camera_status != ACAMERA_OK) {
        LOGE("Failed to open camera. Status: %d", camera_status);
		close();
		return false;
    }
	
	ACameraMetadata* cameraMetadata;
    camera_status = ACameraManager_getCameraCharacteristics(cameraManager, cameraId.c_str(), &cameraMetadata);
    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to get camera metadata. Status: %d", camera_status);
		close();
		return false;
    }
	
	// loadFrameSize(cameraMetadata);
	media_status_t mStatus = AImageReader_new(cameraWidth, cameraHeight, AIMAGE_FORMAT_YUV_420_888, MAX_BUF_COUNT, &reader);
	if (mStatus != AMEDIA_OK) {
		LOGE("ImageReader creation failed with error code: %d", mStatus);
		close();
		return false;
	}

	// The ANativeWindow obtained here does not need to be freed; the AImageReader takes care of that
	ANativeWindow *nativeWindow;
	mStatus = AImageReader_getWindow(reader, &nativeWindow);
	if (mStatus != AMEDIA_OK) {
		LOGE("Could not get ANativeWindow: %d", mStatus);
		close();
		return false;
	}

	camera_status = ACaptureSessionOutputContainer_create(&container);
	if (camera_status != ACAMERA_OK) {
		LOGE("CaptureSessionOutputContainer creation failed with error code: %d", camera_status);
		close();
		return false;
	}

	ANativeWindow_acquire(nativeWindow);
	camera_status = ACaptureSessionOutput_create(nativeWindow, &sessionOutput);
	if (camera_status != ACAMERA_OK) {
		LOGE("CaptureSessionOutput creation failed with error code: %d", camera_status);
		close();
		return false;
	}

	ACaptureSessionOutputContainer_add(container, sessionOutput);

	camera_status = ACameraOutputTarget_create(nativeWindow, &target);
	if (camera_status != ACAMERA_OK) {
		LOGE("CameraOutputTarget creation failed with error code: %d", camera_status);
		close();
		return false;
	}
		
	camera_status = ACameraDevice_createCaptureRequest(cameraDevice, TEMPLATE_PREVIEW, &request);
	if (camera_status != ACAMERA_OK) {
        LOGE("Failed to create preview capture request");
		close();
		return false;
    }
	
	camera_status = ACaptureRequest_addTarget(request, target);
	if (camera_status != ACAMERA_OK) {
		LOGE("Add target to CaptureRequest failed with error code: %d", camera_status);
		close();
		return false;
	} 

	camera_status = ACameraDevice_createCaptureSession(cameraDevice, container, &sessionCallbacks, &session);
	if (camera_status != ACAMERA_OK) {
		LOGE("Failed to create preview session");
		close();
		return false;
	}

	camera_status = ACameraCaptureSession_setRepeatingRequest(session, nullptr, 1, &request, nullptr);
	if (camera_status != ACAMERA_OK) {
		LOGE("CameraCaptureSession set repeating request failed with error code: %d", camera_status);
		close();
		return false;
	}

	AAssetManager* assetManager = app->activity->assetManager;
	if (assetManager == NULL) {
		LOGE("AssetManager not loaded");
		close();
		return false;
	}
	AConfiguration* config = AConfiguration_new();
	AConfiguration_fromAssetManager(config, assetManager);
	
	int32_t screenOrientation = AConfiguration_getOrientation(config);
	ACameraMetadata_const_entry entry = { 0 };
	ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_ORIENTATION, &entry);
	if (screenOrientation == ACONFIGURATION_ORIENTATION_PORT) {
		imageRotation = entry.data.i32[0];
		this->frameWidth = cameraHeight;
		this->frameHeight = cameraWidth;
	} else {
		imageRotation = entry.data.i32[0]-90;
		this->frameWidth = cameraWidth;
		this->frameHeight = cameraHeight;
	}
	LOGI("Camera rotation: %d", imageRotation);

	loaded = true;
	ACameraMetadata_free(cameraMetadata);
	LOGI("Camara opened!");

	opened = true;
	return true;
}

void Camera::close() {
	LOGI("Closing camera");
	opened = false;

	if (!session) return;
	ACameraCaptureSession_close(session);
	session = nullptr;
	
	if (!target) return;
	ACameraOutputTarget_free(target);
	target = nullptr;

	if (!request) return;
	ACaptureRequest_free(request);
	request = nullptr;

	if (!sessionOutput) return;
	ACaptureSessionOutput_free(sessionOutput);
	sessionOutput = nullptr;

	if (!reader) return;
	AImageReader_delete(reader);
	reader = nullptr;

	if (!cameraDevice) return;
	ACameraDevice_close(cameraDevice);
	cameraDevice = nullptr;

	if (!cameraManager) return;
	ACameraManager_delete(cameraManager);
	cameraManager = nullptr;
}

bool Camera::getFrame(OutputArray out) {

	AImage* image;
	media_status_t mStatus = AImageReader_acquireLatestImage(reader, &image);
	if (mStatus != AMEDIA_OK) {
		return false;
	}

	int32_t srcFormat = -1;
	AImage_getFormat(image, &srcFormat);
	if (srcFormat != AIMAGE_FORMAT_YUV_420_888) {
		LOGE("Incorrect image format");
		return false;
	}

	int32_t srcPlanes = 0;
	AImage_getNumberOfPlanes(image, &srcPlanes);
	if (srcPlanes != 3) {
		LOGE("Incorrect number of planes in image data");
		return false;
	}

	int32_t yStride, uvStride;
	uint8_t *yPixel, *uPixel, *vPixel;
	int32_t yLen, uLen, vLen;
	int32_t uvPixelStride;
	AImage_getPlaneRowStride(image, 0, &yStride);
	AImage_getPlaneRowStride(image, 1, &uvStride);
	AImage_getPlaneData(image, 0, &yPixel, &yLen);
	AImage_getPlaneData(image, 1, &uPixel, &uLen);
	AImage_getPlaneData(image, 2, &vPixel, &vLen);
	AImage_getPlanePixelStride(image, 1, &uvPixelStride);

	int32_t uvPlaneDist = uPixel - vPixel;

	int32_t colorFormat;
	if ( (uvPixelStride == 2) && (uvPlaneDist == 1) && (yLen == frameWidth * frameHeight) && (uLen == ((yLen / 2) - 1)) && (uLen == vLen) ) {
		colorFormat = COLOR_FormatYUV420_NV21;
	} else if ( (uvPixelStride == 1) && (uvPlaneDist = vLen) && (yLen == frameWidth * frameHeight) && (uLen == yLen / 4) && (uLen == vLen) ) {
		colorFormat = COLOR_FormatYUV420_YV12;
	} else {
		colorFormat = COLOR_FormatUnknown;
		LOGE("Unsupported format");
		return false;
	}

    std::vector<uint8_t> buffer(yLen + yLen/2);
	// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	if (imageRotation == 0) {
		for (int y = 0; y < yLen; y++) {
			buffer[y] = yPixel[y];
		}

		for (int v = 0; v < vLen; v += uvPixelStride) {
			int u = v + uvPlaneDist;
			buffer[yLen + v] = vPixel[v];
			buffer[yLen + u] = vPixel[u]; 
		}
	} else if (imageRotation == 90) {
		int32_t wY = yLen / yStride;
		int32_t hY = yStride;
		for (int y = 0; y < yLen; y++) {
			buffer[(y%hY)*wY + (wY-1)-y/hY] = yPixel[y];
		}

		int32_t wV = uvPixelStride*wY/2;
		int32_t hV = hY/2;
		for (int i = 0; i < yLen/4; i++) {
			int v = i * uvPixelStride;
			int u = v + uvPlaneDist;
			int newV = (i % hV) * wV + (wV - uvPixelStride) - uvPixelStride * (i / hV);
			int newU = newV + uvPlaneDist;
			buffer[yLen + newV] = vPixel[v];
			buffer[yLen + newU] = vPixel[u]; 
		}
	} else if (imageRotation == 180) {
		for (int y = 0; y < yLen; y++) {
			buffer[yLen - 1 - y] = yPixel[y];
		}

		for (int v = 0; v < vLen; v += uvPixelStride) {
			int u = v + uvPlaneDist;
			int newV = (vLen - 1) - v;
			int newU = newV + uvPlaneDist;
			buffer[yLen + newV] = vPixel[v];
			buffer[yLen + newU] = vPixel[u]; 
		}
	} else if (imageRotation == 270) {
		int32_t wY = yLen / yStride;
		int32_t hY = yStride;
		for (int y = 0; y < yLen; y++) {
			buffer[yLen - 1 - ((y%hY)*wY + (wY-1)-y/hY)] = yPixel[y];
		}

		int32_t wV = uvPixelStride*wY/2;
		int32_t hV = hY/2;
		for (int i = 0; i < yLen/4; i++) {
			int v = i * uvPixelStride;
			int u = v + uvPlaneDist;
			int newV = (vLen - 1) - ((i % hV) * wV + (wV - uvPixelStride) - uvPixelStride * (i / hV));
			int newU = newV + uvPlaneDist;
			buffer[yLen + newV] = vPixel[v];
			buffer[yLen + newU] = vPixel[u]; 
		}
	}
	// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	// LOGI("Orientation fixing time:  %lu", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());

	AImage_delete(image);	

	Mat yuv(frameHeight + frameHeight/2, frameWidth, CV_8UC1, buffer.data());
	if (colorFormat == COLOR_FormatYUV420_YV12) {
		cvtColor(yuv, out, COLOR_YUV2RGB_YV12); 
	} else if (colorFormat == COLOR_FormatYUV420_NV21) {
		cvtColor(yuv, out, COLOR_YUV2RGB_NV21);
	} else {
		LOGE("Unsupported video format: %d", colorFormat);
		return false;
	}

	return true;
}

void Camera::handleCmd(int32_t cmd, android_app* app) {
	switch (cmd) {
        case APP_CMD_INIT_WINDOW:
			LOGI(" > Camera detects INIT WINDOW");
            if (loaded && !opened) {
				open(cameraFacing, cameraWidth, cameraHeight);
            }
            break;
        case APP_CMD_TERM_WINDOW:
			LOGI(" > Camera detects TERM WINDOW");
			close();
            break;
	}
}

bool Camera::isLoaded() {
	return loaded;
}

std::string Camera::getFacingCameraId(CameraFacing cameraFacing) {
    ACameraIdList* cameraIds = nullptr;
    ACameraManager_getCameraIdList(cameraManager, &cameraIds);
	LOGI("Num cameras: %d", cameraIds->numCameras);
	
	acamera_metadata_enum_android_lens_facing_t requestedCam;
	if (cameraFacing == CameraFacing::FRONT) {
		requestedCam = ACAMERA_LENS_FACING_FRONT;
	} else if (cameraFacing == CameraFacing::BACK) {
		requestedCam = ACAMERA_LENS_FACING_BACK;
	} else {
		LOGE("Unknown camera facing");
		return NULL;
	}

	int requestedIndex = 0;
    for (int i = 0; i < cameraIds->numCameras; ++i) {
        const char* id = cameraIds->cameraIds[i];

        ACameraMetadata* metadataObj;
        ACameraManager_getCameraCharacteristics(cameraManager, id, &metadataObj);

        ACameraMetadata_const_entry lensInfo = { 0 };
        ACameraMetadata_getConstEntry(metadataObj, ACAMERA_LENS_FACING, &lensInfo);

		ACameraMetadata_free(metadataObj);
        auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>(lensInfo.data.u8[0]);

        if (facing == requestedCam) {
			requestedIndex = i;
            break;
        }
    }

	std::string requestedId = cameraIds->cameraIds[requestedIndex];
    ACameraManager_deleteCameraIdList(cameraIds);
    return requestedId;
}

void Camera::loadFrameSize(ACameraMetadata* cameraMetadata) {
	ACameraMetadata_const_entry entry;
	ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry);
	for (uint32_t i = 0; i < entry.count; i += 4) {
		int32_t input = entry.data.i32[i + 3];
		int32_t format = entry.data.i32[i + 0];
		
		if (input) {
			continue;
		}

		if (format == AIMAGE_FORMAT_YUV_420_888) {
			frameWidth = entry.data.i32[i + 1];
			frameHeight = entry.data.i32[i + 2];

			LOGI("Frame size detected: %dx%d", frameWidth, frameHeight);
			break;
		}
	}
}

int32_t Camera::getFrameWidth() {
	return frameWidth;
}

int32_t Camera::getFrameHeight() {
	return frameHeight;
}

int32_t Camera::getImageRotation() {
	return imageRotation;
}

/*
 * CALLBACKS
 */

static void onDeviceDisconnected(void* /* ctx */ , ACameraDevice* device) {
	LOGE("Device desconected");
}

static void onDeviceError(void* /* ctx */, ACameraDevice* device, int error) {
	LOGE("Device error detected: %d", error);

    switch (error) {
        case ERROR_CAMERA_IN_USE:
            LOGE("Camera in use");
            break;
        case ERROR_CAMERA_SERVICE:
            LOGE("Fatal Error occured in Camera Service");
            break;
        case ERROR_CAMERA_DEVICE:
            LOGE("Fatal Error occured in Camera Device");
            break;
        case ERROR_CAMERA_DISABLED:
            LOGE("Camera disabled");
            break;
        case ERROR_MAX_CAMERAS_IN_USE:
            LOGE("System limit for maximum concurrent cameras used was exceeded");
            break;
        default:
            LOGE("Unknown Camera Device Error: %#x", error);
    }
}

static void OnSessionClosed(void* /* ctx */, ACameraCaptureSession* session) {
    LOGW("Session %p closed", session);
}

static void OnSessionReady(void* /* ctx */, ACameraCaptureSession* session) {
    LOGW("Session %p ready", session);
}

static void OnSessionActive(void* /* ctx */, ACameraCaptureSession* session) {
    LOGW("Session %p active", session);
}
