#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include "camerathread.h"
#include <QProcess>
#include "SaveImageThread.h"

#include <QUrl>
#include <QString>

class Camera : public QWidget
{
    Q_OBJECT

public:
    Camera(QWidget *parent = nullptr);
    ~Camera();

    QString imageUrl = 0;

private slots:
    void updateImage(const QImage &image);
    void on_btnGoBack_clicked();
    void captureImage();                // 抓拍照片的槽函数
    void captureImage2(int value);      //电平判断抓拍
    void clearCapturedImage();          // 清除抓拍的照片的槽函数
    void timeOutToExecute();            //时间到就执行一次读引脚

private:
    QLabel *frameLabel;                 // 用于显示图像的标签
    CameraThread *cameraThread;
    QPushButton *btnGoBack;
    QLabel *capturedImageLabel;         // 用于显示抓拍照片的标签
    QPushButton *btnCapture;            //抓拍按钮
    QPushButton *btnClearCapture;       //清除抓拍的图片
    QTimer *m_timer;
    SaveImageThread *saveImageThread;
    QString generateImageUrl(const QString &filePath);

signals:
    void returnFirstPage();
    void sendGPIOValue(int value);
    void imageUrlSend(QString abc);
};



#endif // CAMERA_H
