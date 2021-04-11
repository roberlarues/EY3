#ifndef EY3CAMERA_H
#define EY3CAMERA_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <media/NdkImageReader.h>

#include <android_native_app_glue.h>
#include <opencv2/opencv.hpp>
#include <string>

#include "cmd_listener.h"

namespace ey3 {

	enum class CameraFacing {
		FRONT,
		BACK
	};

	/**
	 * Allows to capture real-time images from camera.
	 * - You should open it choosing a camera facing and an existing resolution
	 * - You can take the current frame in RGB by calling 'getFrame'
	 *
	 * NOTE: in portrait mode, resolution values are swapped
	 */
	class Camera : public CmdListener {
		private:
			android_app* app;
			ACameraManager* cameraManager;
			ACameraDevice* cameraDevice;
			AImageReader* reader;
			ACameraCaptureSession *session;
			ACaptureSessionOutput* sessionOutput;
			ACaptureSessionOutputContainer* container;
			ACaptureRequest* request;
			ACameraOutputTarget* target;

			CameraFacing cameraFacing;
			int32_t cameraWidth, cameraHeight; // Original camera resolution
			int32_t frameWidth, frameHeight; // Final frame size (swapped from original in portrait)
			int32_t imageRotation;
			bool loaded = false;
			bool opened = false;

			void loadFrameSize(ACameraMetadata* cameraMetadata);
		public:
			Camera(android_app* app);
			virtual ~Camera();
			bool open(CameraFacing cameraFacing, int32_t cameraWidth, int32_t cameraHeight);
			void close();
			bool getFrame(cv::OutputArray out);

			void handleCmd(int32_t cmd, android_app* app);
			bool isLoaded();
			int32_t getFrameWidth();
			int32_t getFrameHeight();
			int32_t getImageRotation();
			std::string getFacingCameraId(CameraFacing cameraFacing);
	};
}

#endif // EY3CAMERA_H
