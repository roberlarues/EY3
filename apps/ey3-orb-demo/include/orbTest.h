#ifndef ORBTEST_H
#define ORBTEST_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <ey3.h>

using namespace cv;
using namespace ey3;

/**
 * A realtime ORB-based feature matching process.
 * NOTE: output has 2*width than input
 */
class OrbTest: public InputListener {
	private:
		Mat refImage;
		bool hasRefImage;
		bool captureNext;
		std::vector<KeyPoint> keypointsRef, keypointsFrame;
		Mat descriptorsRef, descriptorsFrame;
		Ptr<DescriptorMatcher> matcher;
		Ptr<ORB> detector;
	public:
		OrbTest(int32_t width, int32_t height);
		void handleInput(AInputEvent* event);
		Mat processFrame(Mat frame);
};

#endif // ORBTEST_H
