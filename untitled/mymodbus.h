#ifndef MYMODBUS_H
#define MYMODBUS_H

#include <QWidget>
#include <QModbusClient>
#include <QModbusReply>
#include <QTimer>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QtCharts>

#include <QtCharts/QDateTimeAxis>

#include <QMutex>
#include <QMutexLocker>

#include "my_switch.h"

QT_CHARTS_USE_NAMESPACE
#define AXIS_MAX_Y 10

class MyModbus : public QWidget
{
    Q_OBJECT
public:
    MyModbus(QWidget *parent = nullptr);
    ~MyModbus();

    QString temperature_str = 0;
    QString humidity_str = 0;
    qreal temperature;
    qreal humidity;

private slots:
    void readyRead();
    void on_btnRead_clicked();
    void on_pushButton_clicked();
    void on_timerTimeout();
    void on_pushButton_stop_clicked();
    void on_btnToSecond_clicked();
    void on_btnToSet_clicked();
    void on_btnToCan_clicked();

    void on_switch1(bool checked);
    void on_switch2(bool checked);
    void on_switch3(bool checked);
    void on_switch4(bool checked);
    void on_switch5(bool checked);
    void on_switch6(bool checked);
    void on_switch7(bool checked);
    void on_switch8(bool checked);

private:
    QModbusClient *modbusManager = nullptr;
    QVBoxLayout *btnLayout;
    QPushButton *btnRead;
    QPushButton *pushButton;
    QPushButton *pushButton_stop;
    QPushButton *btnToSecond;
    QPushButton *btnToset;
    QLabel *temp;
    QLabel *hum;

    QPushButton *btnToCan;

    //    QVector<quint16> vector;  //定义一个vector 来摘录reply中收到的数据（reply->result()返回的三qvector，其实三quint16数组）
    quint16 value1;
    quint16 value2;
    QTimer *m_timer;
    quint16 voltage;

    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    QLineSeries *temp_lineSeries;   //曲线
    QLineSeries *hum_lineSeries;   //曲线
    QChart *m_chart;

    QGridLayout *gridLayout;

    QMutex m_mutex; // 添加互斥锁
    //控制继电器
    CustomSwitch *switchButton1;
    CustomSwitch *switchButton2;
    CustomSwitch *switchButton3;
    CustomSwitch *switchButton4;
    CustomSwitch *switchButton5;
    CustomSwitch *switchButton6;
    CustomSwitch *switchButton7;
    CustomSwitch *switchButton8;


signals:
    void goToCameraPage();
    void goToSetPage();
    void temperatureHumidityUpdated(qreal temperature, qreal humidity);
    void goToCan();
};

#endif // MYMODBUS_H
