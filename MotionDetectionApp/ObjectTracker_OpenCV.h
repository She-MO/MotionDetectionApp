#pragma once
#include "DetectedObject_OpenCV.h"
#include "ObjectTracker.cpp"
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
class ObjectTracker_OpenCV :
    public ObjectTracker<DetectedObject_OpenCV, cv::Point>
{
	public:
		void matchCurFrameObjectsToExistingObjects();
		void addObjectToExistingObjects(int curObjIndex, int existObjIndex);
		void addNewObject(int index);
		double distanceBetweenPoints(cv::Point point1, cv::Point point2);
		//ObjectTracker_OpenCV(std::vector<DetectedObject_OpenCV> objects);
};

