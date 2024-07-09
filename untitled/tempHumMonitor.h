#ifndef TEMPHUMMONITOR_H
#define TEMPHUMMONITOR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>
#include "mymodbus.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class TemperatureHumidityMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TemperatureHumidityMonitor(MyModbus *modbus,QWidget *parent = nullptr);
    ~TemperatureHumidityMonitor();

private slots:
    void onSetThresholdsClicked();
    void onControlExternalModuleClicked();
    void onTemperatureHumidityUpdated(qreal temperature, qreal humidity);
    void on_btnGoBack_clicked();

private:
    QLabel *labelTemperature;
    QLabel *labelHumidity;
    QLineEdit *editTemperatureMaxThreshold;
    QLineEdit *editTemperatureMinThreshold;
    QLineEdit *editHumidityMaxThreshold;
    QLineEdit *editHumidityMinThreshold;

    QPushButton *btnSetThresholds;
    QPushButton *btnControlExternalModule;

    double temperatureMaxThreshold = 1000;
    double temperatureMinThreshold = -1000;
    double humidityMaxThreshold = 1000;
    double humidityMinThreshold = -1000;
    QPushButton *returnFirst;
    void initUI(MyModbus *modbus);
    void alert(const QString &message);
    void no_alarm();
    void alarm();
    void analysisJson();

signals:
    void returnFirstPage();

};

#endif // TEMPHUMMONITOR_H
