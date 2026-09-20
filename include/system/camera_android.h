#ifndef EY3CAMERA_H
#define EY3CAMERA_H

#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraMetadata.h>
#include <media/NdkImageReader.h>

#include <opencv2/opencv.hpp>
#include <string>

#include "cmd_listener.h"
#include "window.h"

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
			float fieldOfView;
			bool loaded = false;
			bool opened = false;
			bool permissionRequested = false;

			void loadFrameSize(ACameraMetadata* cameraMetadata);
			void loadFieldOfView(ACameraMetadata* cameraMetadata);
		public:
			Camera();
			virtual ~Camera();
			bool open(CameraFacing cameraFacing, int32_t cameraWidth, int32_t cameraHeight);
			void close();
			bool getFrame(cv::OutputArray out);

			void handleCmd(int32_t cmd, IWindow* window);
			bool isLoaded();
			bool isOpened();
			int32_t getFrameWidth();
			int32_t getFrameHeight();
			int32_t getImageRotation();

			/**
			 * Field of view across the sensor's longer side, in degrees, or
			 * 0 when the camera does not say. Worked out from the lens's
			 * focal length and the size of the sensor, both of which
			 * Camera2 reports, so anything that has to turn pixels into
			 * angles -- HeadTracker, say -- can stop guessing.
			 */
			float getFieldOfView();
			std::string getFacingCameraId(CameraFacing cameraFacing);

			/**
			 * android.permission.CAMERA is a dangerous permission (API 23+): declaring
			 * it in AndroidManifest.xml is not enough, the user must grant it at
			 * runtime. EY3 apps have no Java Activity subclass (NativeActivity,
			 * hasCode="false"), so this is done via JNI calls into the existing
			 * Activity object instead of overriding onRequestPermissionsResult.
			 * There is no async callback available without a custom Java class, so
			 * callers should poll hasPermission() (e.g. retry open() every frame
			 * while !isOpened()) until the user responds to the system dialog.
			 */
			bool hasPermission();
			void requestPermission();
	};
}

#endif // EY3CAMERA_H
