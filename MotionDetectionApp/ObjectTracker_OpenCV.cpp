#include "stdafx.h"
#include "ObjectTracker_OpenCV.h"

void ObjectTracker_OpenCV::matchCurFrameObjectsToExistingObjects() {
	for (int i = 0; i < existingObjects.size(); i++) {

		existingObjects[i].curMatchFoundOrNewBlob = false;

		existingObjects[i].predictNextPosition();
	}

	for (int j = 0; j < curFrameObjects.size(); j++) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingObjects.size(); i++) {

			if (existingObjects[i].stillBeingTracked == true) {

				double dblDistance = distanceBetweenPoints(curFrameObjects[j].centerPositions.back(), existingObjects[i].nextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}
		if (dblLeastDistance < curFrameObjects[j].curDiagonalSize * 0.5) {
			addObjectToExistingObjects(j, intIndexOfLeastDistance);
		}
		else {
			addNewObject(j);
		}
	}

	for (auto& existingObject : existingObjects) {
		if (existingObject.curMatchFoundOrNewBlob == false) {
			existingObject.numOfConsecutiveFramesWithoutAMatch++;
			existingObject.numOfConsecutiveFramesWithAMatch = 0;
		}
		if (existingObject.numOfConsecutiveFramesWithoutAMatch >= 5) {
			existingObject.stillBeingTracked = false;
		}
	}

}

void ObjectTracker_OpenCV::addObjectToExistingObjects(int curObjIndex, int existObjIndex) {
	existingObjects[existObjIndex].currentContour = curFrameObjects[curObjIndex].currentContour;
	existingObjects[existObjIndex].currentBoundingRect = curFrameObjects[curObjIndex].currentBoundingRect;

	existingObjects[existObjIndex].centerPositions.push_back(curFrameObjects[curObjIndex].centerPositions.back());

	existingObjects[existObjIndex].curDiagonalSize = curFrameObjects[curObjIndex].curDiagonalSize;
	existingObjects[existObjIndex].curAspectRatio = curFrameObjects[curObjIndex].curAspectRatio;

	existingObjects[existObjIndex].stillBeingTracked = true;
	existingObjects[existObjIndex].curMatchFoundOrNewBlob = true;
	existingObjects[existObjIndex].numOfConsecutiveFramesWithAMatch = curFrameObjects[curObjIndex].numOfConsecutiveFramesWithAMatch;
	existingObjects[existObjIndex].numOfConsecutiveFramesWithAMatch++;
}

void ObjectTracker_OpenCV::addNewObject(int index) {
	curFrameObjects[index].curMatchFoundOrNewBlob = true;
	existingObjects.push_back(curFrameObjects[index]);
}
double ObjectTracker_OpenCV::distanceBetweenPoints(cv::Point point1, cv::Point point2) {
	int x = abs(point1.x - point2.x);
	int y = abs(point1.y - point2.y);
	return(sqrt(pow(x, 2) + pow(y, 2)));
}
