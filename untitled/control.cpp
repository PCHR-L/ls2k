#include "control.h"

Control::Control()
{

}
Control::~Control()
{

}


void Control::setGPIO(QString port, QString value)
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << port << value);
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
