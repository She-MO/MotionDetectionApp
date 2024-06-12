#include "stdafx.h"

#include <vector>
#include <iostream>
#include <string>
template <typename Img, typename Contour, typename DetectedObj, typename objTracker>
class MotionDetector
{
public:
	virtual void detectMotion(std::string filePath, bool* stopVideo, bool showNumberOfObjects, bool dowloadResult) = 0;
protected:
	objTracker* objectTracker;
	int computeMedianInt(std::vector<int> elements) {
		nth_element(elements.begin(), elements.begin() + elements.size() / 2, elements.end());
		return elements[elements.size() / 2];
	}
	virtual Img computeMedianImg(std::vector<Img> videoFrames) = 0;
	virtual void drawRectsAroundObjects(std::vector<DetectedObj>& contours, Img& frame, bool showNumOfObjects) = 0;
};

