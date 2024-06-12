#include "stdafx.h"
#include "DetectedObject_OpenCV.h"

DetectedObject_OpenCV::DetectedObject_OpenCV(std::vector<cv::Point> _contour) {

    currentContour = _contour;

    currentBoundingRect = cv::boundingRect(currentContour);

    cv::Point currentCenter;

    currentCenter.x = currentBoundingRect.x + currentBoundingRect.width / 2;
    currentCenter.y = currentBoundingRect.y + currentBoundingRect.height / 2;

    centerPositions.push_back(currentCenter);

    curDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));

    curAspectRatio = (double)currentBoundingRect.width / (double)currentBoundingRect.height;

    stillBeingTracked = true;
    curMatchFoundOrNewBlob = true;
    numOfConsecutiveFramesWithAMatch = 1;
    numOfConsecutiveFramesWithoutAMatch = 0;
}

void DetectedObject_OpenCV::predictNextPosition() {
    int numPositions = centerPositions.size();
    int sumOfXChanges = 0;
    int sumOfYChanges = 0;
    double k = 1.0;
    int n = (numPositions > 5) ? 5 : numPositions;
    switch (n) {
        case 3:
            k = 3.0;
            break;
        case 4:
            k = 6.0;
            break;
        case 5:
            k = 10.0;
            break;
    }
    if (n == 1) {
        nextPosition.x = centerPositions[numPositions - 1].x;
        nextPosition.y = centerPositions[numPositions - 1].y;
        return;
    }
    else {
        int koeff = n - 1;
        for (int i = 1; i < n; i++, koeff--) {
            sumOfXChanges += (centerPositions[numPositions - i].x - centerPositions[numPositions - i - 1].x) * koeff;
            sumOfYChanges += (centerPositions[numPositions - i].y - centerPositions[numPositions - i - 1].y) * koeff;
        }
    }
    int deltaX1 = (int)std::round((double)sumOfXChanges / k);
    int deltaY1 = (int)std::round((double)sumOfYChanges / k);
    nextPosition.x = centerPositions.back().x + deltaX1;
    nextPosition.y = centerPositions.back().y + deltaY1;
}
