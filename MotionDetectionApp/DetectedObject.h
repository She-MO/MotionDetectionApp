#include "stdafx.h"
template <typename Contour, typename Rectangle, typename Point>
class DetectedObject
{
public:

    Contour currentContour;

    Rectangle currentBoundingRect;

    std::vector<Point> centerPositions;

    double curDiagonalSize;
    double curAspectRatio;

    bool curMatchFoundOrNewBlob;

    bool stillBeingTracked;
    int numOfConsecutiveFramesWithAMatch;
    int numOfConsecutiveFramesWithoutAMatch;

    Point nextPosition;


    virtual void predictNextPosition() = 0;
};
