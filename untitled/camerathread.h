#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <QMutex>
#include <QMutexLocker>


extern "C"{
#include "v4l2_c-master/v4l2.h"
}

using namespace cv;

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(QObject *parent = nullptr);
    ~CameraThread();
    void run() override;
    QImage getCurrentFrame();

signals:
    void frameCaptured(const QImage &image);

protected:
    pass_data pd;
    bool initCamera();
    void deinitCamera();
    QImage captureFrame();
    QImage currentFrame; // 当前捕获的帧

private:
    cv::Mat QImage2cvMat(QImage image);         //QImage转换为Mat
    QImage cvMat2QImage(const cv::Mat& mat);    //Mat转换为QImage

    QMutex m_mutex; // 添加互斥锁

};

#endif // CAMERATHREAD_H
