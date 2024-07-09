#include "SaveImageThread.h"
#include <QDebug>
SaveImageThread::SaveImageThread(QObject *parent) : QThread(parent)
{
}

SaveImageThread::~SaveImageThread()
{
    wait();
}

void SaveImageThread::saveImage(const QImage &image, const QString &filePath)
{
    imageToSave = image;
    fileToSave = filePath;

    start(); // 启动线程执行保存操作
}

void SaveImageThread::run()
{
    if (!imageToSave.isNull() && !fileToSave.isEmpty()) {
        bool success = imageToSave.save(fileToSave,"PNG");
        if (!success) {
            qDebug() << "Failed to save image to" << fileToSave;
        }
    }
}



