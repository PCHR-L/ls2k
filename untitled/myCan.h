#ifndef MYCAN_H
#define MYCAN_H

#include <QCanBusDevice>
#include <QCanBus>
#include <QPushButton>
#include <QTextBrowser>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QDebug>

class MyCan : public QWidget
{
    Q_OBJECT
public:
    MyCan(QWidget *parent = nullptr);
    ~MyCan();

private:
    QCanBusDevice *canDevice;
    QTextBrowser *textBrowser;
    QLineEdit *lineEdit;            //发送数据
    QPushButton *pushButton[2];
    QComboBox *comboBox[3];
    QLabel *label[4];
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QWidget *funcWidget;            //设置功能区域

    QPushButton *btnGoBack;

    void layoutInit();
    void pluginItemInit();
    void bitrateItemInit();

private slots:
    void sendFrame();               //发送消息
    void receivedFrames();          //接收消息
    void pluginChanged(int);
    void canDeviceErrors(QCanBusDevice::CanBusError) const; //错误处理
    void connectDevice();           //连接或断开can

    void on_btnGoBack_clicked();

signals:
    void returnFirstPage();
};

#endif // MYCAN_H
