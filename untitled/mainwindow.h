#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include "mymodbus.h"
#include "camera.h"
#include "tempHumMonitor.h"
#include "mqttclient.h"
#include "datasend.h"
#include "httpserver.h"
#include "myCan.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStackedWidget *stackedWidget;
    QTimer *mytimer;
    MyModbus *mymodbus;
    Camera *mycamera;
    TemperatureHumidityMonitor *thw;
    DataSender *datasend;
    HttpServer *server;
    MyCan *mycan;

    static void fortest_handler(void* client, message_data_t* msg);
//    static void fasong_handler(void* client, message_data_t* msg);
private slots:
    void handleGoToCameraPage();
    void handleGoToSetpage();
    void handleReturnFirstPage();
    void handleGoToCan();
};
#endif // MAINWINDOW_H
