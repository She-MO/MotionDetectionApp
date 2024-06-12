#pragma once
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "DetectedObject.h"
class DetectedObject_OpenCV : public DetectedObject<std::vector<cv::Point>, cv::Rect, cv::Point>
{
	public:
		DetectedObject_OpenCV(std::vector<cv::Point> _contour);
		void predictNextPosition();
};

