#ifndef MYMODBUS_H
#define MYMODBUS_H

#include <QWidget>
#include <QModbusClient>
#include <QModbusReply>
#include <QTimer>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QtCharts>

#include "control.h"
#include <QtCharts/QDateTimeAxis>

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
    void on_btnControlDevice1_open();
    void on_btnControlDevice1_close();
    void on_btnControlDevice2_open();
    void on_btnControlDevice2_close();
    void on_btnControlDevice3_open();
    void on_btnControlDevice3_close();
    void on_btnControlDevice4_open();
    void on_btnControlDevice4_close();

private:
    QModbusClient *modbusManager = nullptr;
    QVBoxLayout *btnLayout;
    QPushButton *btnRead;
    QPushButton *pushButton;
    QPushButton *pushButton_stop;
    QPushButton *btnToSecond;
    QPushButton *btnToset;
    QLineEdit *tempLineEdit;
    QLineEdit *humLineEdit;
    QLabel *temp;
    QLabel *hum;

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

signals:
    void goToCameraPage();
    void goToSetPage();
    void temperatureHumidityUpdated(qreal temperature, qreal humidity);
};

#endif // MYMODBUS_H
