#include "camerathread.h"
#include <QImage>
#include <QDebug>
#include <QProcess>
#include <QDateTime>

CameraThread::CameraThread(QObject *parent) : QThread(parent), pd{}
{
    // 初始化摄像头
    if (!initCamera()) {
        qDebug() << "Failed to initialize camera";
    }
}

CameraThread::~CameraThread()
{
    deinitCamera();
    if (isRunning()) {
        quit();
        wait();
    }
}

void CameraThread::run()
{
    while (!isInterruptionRequested()) {
        QImage frame = captureFrame();
        currentFrame = frame;
        emit frameCaptured(frame);

    }
}

bool CameraThread::initCamera()
{
    //选定摄像头
    pd.dev_name = "/dev/video0";
    //初始化摄像头
    int flag = init_dev(&pd);
    qDebug() << "摄像头初始化成功";
    //判断摄像头是否初始化成功
    if(flag == -1 ){
        qDebug() << "no device";
        return false;
    }else if(flag == -2){
        qDebug() << "device is wrong";
        return false;
    }else if(flag == -3){
        qDebug() << "can not open device";
        return false;
    }
    return true;
}

void CameraThread::deinitCamera()
{
    if (pd.fd != -1) {
        stop_capturing(&pd);
        stop_dev(&pd);
        close_device(&pd);
    }
}

QImage CameraThread::captureFrame()
{
    // 确保图像处理和显示逻辑是高效的
    cv::Mat load_image, gray_image;
    QImage image_raw;
    //读取一帧图像
    read_frame(&pd);
    if (pd.buf.index < pd.n_buffers){
        //把图像装载到QImage
        image_raw.loadFromData((const uchar *)pd.buffers[pd.buf.index].start, pd.buffers[pd.buf.index].length);
    }
    //返还空间
    return_data(&pd);
    load_image = QImage2cvMat(image_raw);
    cv::cvtColor(load_image, gray_image, CV_BGR2GRAY);
    equalizeHist(gray_image, gray_image);

    QImage image_ok = cvMat2QImage(load_image);
    return image_ok;
}

QImage CameraThread::cvMat2QImage(const cv::Mat& mat)
{
    //Mat类型为8为无符号，通道数为1
    if(mat.type() == CV_BLUR){
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for(int i = 0; i < 256; i++){
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row++){
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }else if(mat.type() == CV_8UC3){
        const uchar *pSrc = (const uchar *)mat.data;
        QImage image(pSrc, mat.cols, mat.rows,mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }else if(mat.type() == CV_8UC4){    //Mat类型为8位无符号，通道数为4
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }else{
        qDebug() << "ERROR: Mat could not be converted to QImage";
        return QImage();
    }}

cv::Mat CameraThread::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    switch(image.format()){
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat , CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage CameraThread::getCurrentFrame()
{
    QImage frameCopy = currentFrame.copy(); // 创建当前帧的副本
    return frameCopy;
}

