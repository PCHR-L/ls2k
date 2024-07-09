#ifndef SAVEIMAGETHREAD_H
#define SAVEIMAGETHREAD_H

#include <QThread>
#include <QImage>
#include <QString>

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
};


#endif // SAVEIMAGETHREAD_H
