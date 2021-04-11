#include "orbTest.h"
#include <string>
#include <chrono>
#include <ey3.h>

OrbTest::OrbTest(int32_t width, int32_t height): hasRefImage(false), captureNext(false) {
	matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
	detector = ORB::create();
	refImage = Mat::zeros(height, width, CV_8UC3);

	int fontFace = FONT_HERSHEY_SIMPLEX;
	double fontScale = 2;
	int thickness = 3;
	int baseline=0;
	std::string text = "Press to capture";
	Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
	putText(refImage, text, Point(width/2.f, height/2.f) - Point(textSize.width/2.f, textSize.height/2.f), fontFace, fontScale, Scalar::all(255), thickness, 8);
}

void OrbTest::handleInput(AInputEvent* event) {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		switch(AInputEvent_getSource(event)){
			case AINPUT_SOURCE_TOUCHSCREEN:
				int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
				switch(action){
					case AMOTION_EVENT_ACTION_DOWN: {
						captureNext = true;
						break;
					}
				}
			break;
		}
	}
}

Mat OrbTest::processFrame(Mat frame) {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	if (captureNext) {
		refImage = frame.clone();
		captureNext = false;
		hasRefImage = true;
		detector->detectAndCompute(refImage, noArray(), keypointsRef, descriptorsRef);
	}

	std::vector<DMatch> matches;
	std::vector<char> mask;
	if (hasRefImage) {
		detector->detectAndCompute(frame, noArray(), keypointsFrame, descriptorsFrame);
		matcher->match(descriptorsFrame, descriptorsRef, matches);
		
		std::vector<Point2f> framePoints;
	    std::vector<Point2f> refPoints;
		for (int i = 0; i < matches.size(); i++) {
			framePoints.push_back(keypointsFrame[matches[i].queryIdx].pt);
			refPoints.push_back(keypointsRef[matches[i].trainIdx].pt);
		}

		int numInliers = 0;
		if (matches.size() > 20) {
			Mat homography = findHomography(framePoints, refPoints, RANSAC, 3, mask);
			numInliers = std::count(mask.begin(), mask.end(), true);
		}	

		if (numInliers < 35) {
			matches.clear();
			mask.clear();
		}
	}
	
	Mat outImg;
	drawMatches(frame, keypointsFrame, refImage, keypointsRef, matches, outImg, Scalar(0, 255, 255), Scalar(0, 255, 255), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	unsigned long microSecs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	putText(outImg, "Exec time = " + std::to_string(microSecs) + "us", Point(20, 30), 0, 1, Scalar::all(255));
	return outImg;	
}
