#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <ctime>
#include <thread>
#include <ppl.h>
#include "MotionDetector.cpp"
#include "DetectedObject_OpenCV.h"
#include "ObjectTracker_OpenCV.h"
class MotionDetector_OpenCV :
	public MotionDetector<cv::Mat, std::vector<cv::Point>, DetectedObject_OpenCV, ObjectTracker<DetectedObject_OpenCV, cv::Point>>
{
public:
	void detectMotion(std::string filePath, bool* stopVideo, bool showNumberOfObjects, bool dowloadResult);
	cv::Mat computeMedianImg(std::vector<cv::Mat> videoFrames);
	void drawRectsAroundObjects(std::vector<DetectedObject_OpenCV>& objects, cv::Mat& frame, bool showNumOfObjects);
	MotionDetector_OpenCV();
	MotionDetector_OpenCV(ObjectTracker<DetectedObject_OpenCV, cv::Point>* objTracker);
	~MotionDetector_OpenCV();
};

