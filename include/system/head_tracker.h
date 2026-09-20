#ifndef EY3HEAD_TRACKER_H
#define EY3HEAD_TRACKER_H

#include <opencv2/core.hpp>
#include <opencv2/objdetect/face.hpp>

#include "asset_loader.h"
#include "vec3.h"

namespace ey3 {

	/** What the face detector last saw, in the coordinates of the frame. */
	struct FaceDetection {
		cv::Rect box;
		cv::Point2f rightEye;
		cv::Point2f leftEye;
		float score;
	};

	/**
	 * Follows the player's face with the front camera and says where their
	 * eyes are, in centimetres, with the origin at the centre of the screen:
	 * +x to the player's right, +y up, +z towards the player.
	 *
	 * That is what a head-coupled ("fishtank") perspective needs: feed the
	 * eye position into Mat4::frustum and the screen stops being a flat
	 * picture and becomes a window into the scene.
	 *
	 * Faces are found with YuNet, the small neural detector that ships with
	 * OpenCV, loaded from a model file in the app's assets. It gives the two
	 * eyes directly, so the distance comes from how far apart they look --
	 * a far steadier measure than the size of a head, and it does not care
	 * what colour the wall behind the player is.
	 *
	 * The result is smoothed with a One Euro filter, which is what keeps the
	 * scene from shaking without adding the lag a plain average would.
	 */
	class HeadTracker {
		private:
			// One axis of the One Euro filter: an adaptive low-pass whose
			// cutoff rises with speed, so it is steady when the head is still
			// and quick when it moves.
			struct Axis {
				float value;
				float speed;
				bool started;

				float filter(float sample, float deltaTime, float minCutoff, float beta);
			};

			Axis axisX;
			Axis axisY;
			Axis axisZ;

			cv::Ptr<cv::FaceDetectorYN> detector;
			FaceDetection detection;
			float detectionMillis;

			Vec3 eyePosition;
			Vec3 restingPosition;
			bool faceFound;
			float timeWithoutFace;

			float fieldOfViewDegrees;
			float cameraOffsetX;
			float cameraOffsetY;
			float eyeDistanceCm;
			bool mirrored;

			bool findFace(const cv::Mat& frame);

		public:
			HeadTracker();

			/**
			 * Loads the detector's model, which must be in the app's assets
			 * (get it from OpenCV's model zoo: face_detection_yunet). Needs
			 * no GL context, so it can be done as the app starts. Returns
			 * false if the model is missing, and then no face is ever found.
			 */
			bool init(AssetLoader* assetLoader, const char* modelAsset);

			/**
			 * Field of view of the front camera across the longer side of
			 * the image, in degrees. The longer side, not the horizontal
			 * one, because Android hands over frames already turned upright
			 * for a portrait screen. The default (60) is typical of a phone's
			 * front camera and of a laptop webcam; a wrong value shows up as
			 * the effect being too strong or too weak, not as breakage.
			 */
			void setFieldOfView(float degrees);

			/** Where the lens sits relative to the centre of the screen, in cm. */
			void setCameraOffset(float xCm, float yCm);

			/**
			 * Distance between the player's pupils, in cm. It is what sets
			 * the distance scale; 6.3 is the adult average.
			 */
			void setEyeDistance(float centimetres);

			/** Whether the camera image is mirrored, as a front camera's is. */
			void setMirrored(bool mirrored);

			/** Where the eyes are assumed to be while no face is visible. */
			void setRestingPosition(const Vec3& position);

			/**
			 * Feeds one frame from the front camera. It must be RGB, which
			 * is what Camera::getFrame gives on both platforms.
			 */
			void update(const cv::Mat& frame, float deltaTime);

			Vec3 getEyePosition() const;
			bool hasFace() const;

			/** What the detector last saw, for drawing a debug overlay. */
			const FaceDetection& getDetection() const;

			/**
			 * How long the last detection took, in milliseconds. This is the
			 * bulk of the lag between moving your head and the view
			 * following, so it is worth watching on a real device.
			 */
			float getDetectionMillis() const;
	};
}

#endif // EY3HEAD_TRACKER_H
