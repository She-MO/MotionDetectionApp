#include "stdafx.h"
#include <vector>
#include <iostream>
#include <string>
template <typename DetectedObj, typename Point>
class ObjectTracker {
	public:
		std::vector<DetectedObj> existingObjects;
		std::vector<DetectedObj> curFrameObjects;
		virtual void matchCurFrameObjectsToExistingObjects() = 0;
		virtual void addObjectToExistingObjects(int curObjIndex, int existObjIndex) = 0;
		virtual void addNewObject(int index) = 0;
		virtual double distanceBetweenPoints(Point point1, Point point2) = 0;
};