#include "camera.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

Camera::Camera(QWidget *parent)
    : QWidget(parent)
{
    m_timer = new QTimer(this);

    saveImageThread = new SaveImageThread(this);

    btnGoBack = new QPushButton("返回",this);
    btnCapture = new QPushButton("抓拍", this);
    btnClearCapture = new QPushButton("清除照片", this);
    cameraThread = new CameraThread(this);

    frameLabel = new QLabel(this);
    frameLabel->resize(400,400);
    frameLabel->setStyleSheet("QLabel { border: 1px solid black; }"); // 给标签添加边框
    capturedImageLabel = new QLabel(this);
    capturedImageLabel->resize(400,400);
    capturedImageLabel->setStyleSheet("QLabel { border: 1px solid black; }"); // 给标签添加边框

    capturedImageLabel->setFixedSize(frameLabel->size());
    frameLabel->setFixedSize(capturedImageLabel->size());

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(frameLabel, 0, 0);
    gridLayout->addWidget(capturedImageLabel, 0, 1);

    // 设置布局属性，使标签内容居中
    gridLayout->setAlignment(Qt::AlignCenter);

    gridLayout->setSpacing(10); // 设置栅格布局的间距


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(btnCapture);
    layout->addWidget(btnClearCapture);
    layout->addWidget(btnGoBack);

    gridLayout->addLayout(layout,1,0);
    setLayout(gridLayout); // 应用栅格布局
    connect(btnGoBack,&QPushButton::clicked,this,&Camera::on_btnGoBack_clicked);
    connect(cameraThread, &CameraThread::frameCaptured, this, &Camera::updateImage);
    connect(btnCapture, &QPushButton::clicked, this, &Camera::captureImage);
    connect(btnClearCapture, &QPushButton::clicked, this, &Camera::clearCapturedImage);
    connect(m_timer,&QTimer::timeout,this,&Camera::timeOutToExecute);
    connect(this,&Camera::sendGPIOValue,this,&Camera::captureImage2);
    m_timer->start(1000);           //1秒读一次GPIO

    cameraThread->start();

}


Camera::~Camera()
{
    cameraThread->quit();
    cameraThread->wait();
    saveImageThread->quit();
    saveImageThread->wait();
    delete cameraThread;
    delete frameLabel;
    delete saveImageThread;
}

void Camera::updateImage(const QImage &image)
{
    frameLabel->setPixmap(QPixmap::fromImage(image).scaled(frameLabel->size(), Qt::KeepAspectRatio));
}

void Camera::on_btnGoBack_clicked()
{
    emit returnFirstPage();
}

void Camera::captureImage() {
    QImage currentFrame = cameraThread->getCurrentFrame();
    capturedImageLabel->setPixmap(QPixmap::fromImage(currentFrame));

}
void Camera::captureImage2(int value) {
    if(value == 0) { //经测试，当红外对管检测到物体后GPIO为低电平
        QImage currentFrame = cameraThread->getCurrentFrame();
        capturedImageLabel->setPixmap(QPixmap::fromImage(currentFrame));
        QString filePath = "/home/pchrl/capture/capture_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +".png"; // 替换为实际的文件路径
        saveImageThread->saveImage(currentFrame, filePath);
        imageUrl = generateImageUrl(filePath);
        emit imageUrlSend(imageUrl);
    }
}

void Camera::clearCapturedImage() {
    if (capturedImageLabel) {
        capturedImageLabel->clear(); // 清除QLabel中的图片
    }
}

void Camera::timeOutToExecute()
{
    QProcess *m_process = new QProcess(this);
    m_process->start("/home/pchrl/myLoongson/gpio_in", QStringList() << "58");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(m_process->state() == QProcess::Running) {
        m_process->terminate();  // 终止子进程执行
        m_process->waitForFinished();  // 等待子进程完成
        if (m_process->state() == QProcess::Running) {
            m_process->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    m_process->deleteLater();

    QFile file("/sys/class/gpio/gpio58/value");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开GPIO值文件:" << file.errorString();
        return;
    }
    QTextStream in(&file);
    QString value = in.readLine().trimmed(); // 读取并去除空白字符
    qDebug() << "GPIO值:" << value.toInt();
    file.close();

    emit sendGPIOValue(value.toInt());
}

QString Camera::generateImageUrl(const QString &filePath) {
    // 如果是本地文件，直接使用file://协议
    QUrl url = QUrl::fromLocalFile(filePath);
    return url.toString();
}
