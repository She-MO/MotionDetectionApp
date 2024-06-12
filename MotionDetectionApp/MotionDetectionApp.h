#pragma once

#include <QtWidgets/QWidget>
#include "ui_MotionDetectionApp.h"

class MotionDetectionApp : public QWidget
{
    Q_OBJECT

public:
    MotionDetectionApp(QWidget *parent = nullptr);
    ~MotionDetectionApp();

private:
    Ui::MotionDetectionAppClass ui;
private slots:
    void on_chooseFileButton_clicked();
    void on_submitButton_clicked();
    void on_stopVideoButton_clicked();
};
