#include "stdafx.h"
#include "MotionDetectionApp.h"
#include "MotionDetector_OpenCV.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MotionDetectionApp w;
    w.show();
    return a.exec();
}

