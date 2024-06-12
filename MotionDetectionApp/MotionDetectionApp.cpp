#include "stdafx.h"
#include "MotionDetectionApp.h"
#include "MotionDetector_OpenCV.h"
#include "thread"

bool stopVideo = false;
bool stopVideoButtonWasClicked = false;
auto toUtf16 = QStringDecoder(QStringDecoder::System);
MotionDetectionApp::MotionDetectionApp(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

MotionDetectionApp::~MotionDetectionApp()
{}

void MotionDetectionApp::on_chooseFileButton_clicked() {
    ui.currentActionInfo->clear();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Mp4 and Avi Files (*.mp4 *.avi)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui.filePath->setText(fileName);
}
QString convertStringToUtf16(std::string message) {
    QByteArray* arr = new QByteArray(message.c_str(), message.length());
    QString string = toUtf16(*arr);
    return string;
}
void MotionDetectionApp::on_submitButton_clicked() {
    ui.currentActionInfo->clear();
    ui.currentActionInfo->setText(convertStringToUtf16("Триває обробка відео..."));
    ui.currentActionInfo->repaint();
    if ((ui.filePath->text()).toStdString().length() < 1) {
        ui.currentActionInfo->setText(convertStringToUtf16("Під час відкриття відео виникла помилка"));
        return;
    }
    stopVideoButtonWasClicked = false;
    stopVideo = false;
    MotionDetector_OpenCV* motionDetector = new MotionDetector_OpenCV();
    //MotionDetector_OpenCV* motionDetector = new MotionDetector_OpenCV(new ObjectTracker_OpenCV());
    std::string path = (ui.filePath->text()).toStdString();
    bool showNumberOfObjects = ui.showNumberOfMovingObjects->isChecked();
    bool downloadResult = ui.dowloadVid->isChecked();
    ui.stopVideoButton->setEnabled(true);
    motionDetector->detectMotion(path, &stopVideo, showNumberOfObjects, downloadResult);
    if (stopVideo && !stopVideoButtonWasClicked && downloadResult) {
        ui.currentActionInfo->setText(convertStringToUtf16("Під час відкриття або завантаження \nвідео виникла помилка"));
    } else if (stopVideo && !stopVideoButtonWasClicked) {
        ui.currentActionInfo->setText(convertStringToUtf16("Під час відкриття відео виникла помилка"));
    }
    else if (downloadResult) {
        int i = path.find_last_of('/');
        std::string downloadPath = std::string(path, 0, i);
        ui.currentActionInfo->setText(convertStringToUtf16("Відео було успішно завантажено до \n" + downloadPath));
    }
    else {
        ui.currentActionInfo->clear();
    }
    ui.stopVideoButton->setEnabled(false);
    delete motionDetector;
}
void MotionDetectionApp::on_stopVideoButton_clicked() {
    ui.currentActionInfo->clear();
    stopVideo = true;
    stopVideoButtonWasClicked = true;
}