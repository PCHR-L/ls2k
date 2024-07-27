#ifndef SAVEIMAGETHREAD_H
#define SAVEIMAGETHREAD_H

#include <QThread>
#include <QImage>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class SaveImageThread : public QThread
{
    Q_OBJECT

public:
    SaveImageThread(QObject *parent = nullptr);
    ~SaveImageThread();

    void saveImage(const QImage &image, const QString &filePath);

protected:
    void run() override;

private:
    QImage imageToSave;
    QString fileToSave;

    QMutex m_mutex; // 添加互斥锁
};


#endif // SAVEIMAGETHREAD_H
