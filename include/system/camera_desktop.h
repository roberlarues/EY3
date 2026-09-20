#ifndef EY3CAMERA_H
#define EY3CAMERA_H

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include "cmd_listener.h"
#include "window.h"

namespace ey3 {

	enum class CameraFacing {
		FRONT,
		BACK
	};

	/**
	 * Captures real-time frames from the system's default webcam via OpenCV's
	 * VideoCapture. Desktop webcams have no front/back distinction, no
	 * windowing-tied lifecycle and no runtime permission prompt, so
	 * CameraFacing is ignored and hasPermission()/requestPermission() are
	 * no-ops -- kept only so app code stays identical to the Android build.
	 */
	class Camera : public CmdListener {
		private:
			cv::VideoCapture capture;
			int32_t deviceIndex;

			CameraFacing cameraFacing;
			int32_t cameraWidth, cameraHeight; // Requested capture resolution
			int32_t frameWidth, frameHeight; // Actual negotiated frame size
			bool loaded = false;
			bool opened = false;

		public:
			Camera(int32_t deviceIndex = 0);
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
			 * Field of view across the sensor's longer side, in degrees.
			 * Always 0 here: V4L2 webcams do not report their optics, so a
			 * caller keeps whatever default it has. Android does report it.
			 */
			float getFieldOfView();

			bool hasPermission();
			void requestPermission();
	};
}

#endif // EY3CAMERA_H
