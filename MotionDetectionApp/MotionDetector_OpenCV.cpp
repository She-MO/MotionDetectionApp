#include "stdafx.h"
#include "MotionDetector_OpenCV.h"
#include "ObjectTracker_OpenCV.h"


cv::Mat MotionDetector_OpenCV::computeMedianImg(std::vector<cv::Mat> videoFrames) {
	int rows = videoFrames[0].rows;
	int cols = videoFrames[0].cols;
	cv::Scalar sc(0, 0, 0);
	cv::Mat medianImg(rows, cols, CV_8UC3, sc);

	Concurrency::parallel_for(size_t(0), size_t(rows), [&](size_t row) {
		for (int col = 0; col < videoFrames[0].cols; col++) {
			std::vector<int> elements_B;
			std::vector<int> elements_G;
			std::vector<int> elements_R;
			for (int imgNumber = 0; imgNumber < videoFrames.size(); imgNumber++) {
				int B = videoFrames[imgNumber].at<cv::Vec3b>(row, col)[0];
				int G = videoFrames[imgNumber].at<cv::Vec3b>(row, col)[1];
				int R = videoFrames[imgNumber].at<cv::Vec3b>(row, col)[2];
				elements_B.push_back(B);
				elements_G.push_back(G);
				elements_R.push_back(R);
			}

			medianImg.at<cv::Vec3b>(row, col)[0] = computeMedianInt(elements_B);
			medianImg.at<cv::Vec3b>(row, col)[1] = computeMedianInt(elements_G);
			medianImg.at<cv::Vec3b>(row, col)[2] = computeMedianInt(elements_R);
		}

		});

	return medianImg;
}

void MotionDetector_OpenCV::drawRectsAroundObjects(std::vector<DetectedObject_OpenCV>& objects, cv::Mat& frame, bool showNumOfObjects) {
	int numOfObjects = 0;
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i].stillBeingTracked && objects[i].numOfConsecutiveFramesWithAMatch > 1) {
			cv::rectangle(frame, objects[i].currentBoundingRect, cv::Scalar(255, 0, 0), 2);
			numOfObjects++;
		}
	}
	if (showNumOfObjects) {
		std::string str = std::to_string(numOfObjects);
		putText(frame, str, cv::Point(20, 30), cv::FONT_HERSHEY_TRIPLEX, 1.0, cv::Scalar(10, 10, 10), 2);
	}
}

void MotionDetector_OpenCV::detectMotion(std::string filePath, bool* stopVideo, bool showNumberOfObjects, bool downloadResult) {
	srand(time(NULL));
	char checkForEscKey;
	cv::VideoCapture submittedVideo(filePath);
	if (!submittedVideo.isOpened()) {
		*stopVideo = true;
		return;
	}
	std::vector<cv::Mat> frames;
	cv::Mat frame;
	int numberOfFrames = submittedVideo.get(cv::CAP_PROP_FRAME_COUNT);
	int numOfFramesForMedianFrame = (numberOfFrames > 50) ? 50 : numberOfFrames;
	for (int i = 0; i < numOfFramesForMedianFrame; i++)
	{
		int fid = rand() % numberOfFrames;
		submittedVideo.set(cv::CAP_PROP_POS_FRAMES, fid);
		cv::Mat frame;
		submittedVideo >> frame;
		if (frame.empty())
			continue;
		frames.push_back(frame);
	}

	cv::Mat medianFrame = computeMedianImg(frames);

	cv::VideoWriter writer;
	if (downloadResult) {
		int i = filePath.find_last_of('/');

		std::string downloadPath = filePath;

		downloadPath.insert(i + 1, "MD_");

		int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');
		double fps = submittedVideo.get(cv::CAP_PROP_FPS);
		bool isColor = (medianFrame.type() == CV_8UC3);
		writer.open(downloadPath, codec, fps, medianFrame.size(), isColor);
		if (!writer.isOpened()) {
			*(stopVideo) = true;
			return;
		}
	}

	submittedVideo.set(cv::CAP_PROP_POS_FRAMES, 0);

	cv::Mat grayMedianFrame;
	cvtColor(medianFrame, grayMedianFrame, cv::COLOR_BGR2GRAY);

	// Loop over all frames
	int frameCount = 0;
	int consecutiveFrames = 3;
	std::vector<cv::Mat> frameDiffList;
	std::vector<cv::Mat> frameList;
	std::vector<cv::Mat> resultFrames;
	cv::Ptr<cv::BackgroundSubtractor> pBackSub;
	pBackSub = cv::createBackgroundSubtractorMOG2(500, 30.0, true);
	cv::Mat fgMask;

	bool blnFirstFrame = true;
	cv::VideoCapture vidWithDetectedObjects;
	while (1) {
		submittedVideo >> frame;

		if (frame.empty())
			break;
		cv::Mat gframe;
		pBackSub->apply(frame, fgMask);
		cvtColor(frame, gframe, cv::COLOR_BGR2GRAY);
		GaussianBlur(gframe, gframe, cv::Size(5, 5), 0);
		if (frameCount % consecutiveFrames == 0) {
			frameDiffList.clear();
			frameList.clear();
		}
		frameList.push_back(frame);
		frameCount++;
		cv::Mat dframe;
		absdiff(gframe, grayMedianFrame, dframe);

		threshold(dframe, dframe, 30, 255, cv::THRESH_BINARY);
		threshold(fgMask, fgMask, 210, 255, cv::THRESH_BINARY);

		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
		morphologyEx(fgMask, fgMask, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 2);
		morphologyEx(dframe, dframe, cv::MORPH_OPEN, kernel);
		dilate(fgMask, fgMask, kernel, cv::Point(-1, -1), 2);

		dframe += fgMask;
		frameDiffList.push_back(dframe);
		if (!downloadResult) {
			cv::namedWindow("Motion detection app", cv::WINDOW_AUTOSIZE);
		}
		if (frameDiffList.size() == consecutiveFrames) {

			cv::Mat drawing = frame.clone();
			cv::Mat framesSum = cv::Mat::zeros(frame.size(), CV_8UC1);
			for (int i = 0; i < consecutiveFrames; i++) {
				framesSum += frameDiffList[i];
			}
			std::vector<std::vector<cv::Point> > contours;
			findContours(framesSum, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
			std::vector<std::vector<cv::Point> > contours_poly(contours.size());
			std::vector<cv::Rect> boundRect(contours.size());
			std::vector<std::vector<cv::Point> > convexHulls(contours.size());
			cv::Scalar color = cv::Scalar(255, 255, 255);
			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(contours[i], contours_poly[i], 3, true);
				cv::convexHull(contours_poly[i], convexHulls[i]);
				boundRect[i] = boundingRect(contours_poly[i]);
			}
			for (auto& convexHull : convexHulls) {
				DetectedObject_OpenCV possibleObject(convexHull);

				if (possibleObject.currentBoundingRect.area() > 200 &&
					possibleObject.curAspectRatio > 0.2 &&
					possibleObject.curAspectRatio < 4.0 &&
					possibleObject.currentBoundingRect.width > 20 &&
					possibleObject.currentBoundingRect.height > 20 &&
					possibleObject.curDiagonalSize > 40.0 &&
					(cv::contourArea(possibleObject.currentContour) / (double)possibleObject.currentBoundingRect.area()) > 0.50) {
					objectTracker->curFrameObjects.push_back(possibleObject);
				}
			}
			if (blnFirstFrame == true) {
				for (auto& currentFrameBlob : objectTracker->curFrameObjects) {
					objectTracker->existingObjects.push_back(currentFrameBlob);
				}
				blnFirstFrame = false;
			}
			else {
				objectTracker->matchCurFrameObjectsToExistingObjects();
			}
			for (int k = 0; k < consecutiveFrames; k++) {
				drawRectsAroundObjects(objectTracker->existingObjects, frameList[k], showNumberOfObjects);
				if (downloadResult) {
					resultFrames.push_back(frameList[k]);
					writer.write(frameList[k]);
				}
				else {
					imshow("Motion detection app", frameList[k]);
					checkForEscKey = cv::waitKey(20);
				}
			}
			objectTracker->curFrameObjects.clear();
			if (*(stopVideo)) {               
				cv::destroyAllWindows();                         
				break;
			}
		}
	}


	submittedVideo.release();
	cv::destroyAllWindows();
	return;
}
MotionDetector_OpenCV::MotionDetector_OpenCV() {
	this->objectTracker = new ObjectTracker_OpenCV();
}

MotionDetector_OpenCV::MotionDetector_OpenCV(ObjectTracker<DetectedObject_OpenCV, cv::Point>* objTracker) {
	this->objectTracker = objTracker;
}
MotionDetector_OpenCV::~MotionDetector_OpenCV() {
	delete objectTracker;
}
