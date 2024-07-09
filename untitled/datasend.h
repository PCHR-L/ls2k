#ifndef DATASENDTHREAD_H
#define DATASENDTHREAD_H

#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "mqttclient.h"
#include "mymodbus.h"

class DataSender : public QObject
{
    Q_OBJECT
public:
    DataSender(QObject *parent = nullptr,MyModbus *modbus = nullptr,mqtt_client_t *client = nullptr, const char *topic = nullptr);
    ~DataSender();
    void setParameters(double wendu, double shidu);
protected:

    // 创建 JSON 对象并填充数据
    QJsonObject createJsonObject(double temperature, double humidity);
    // 将 QJsonObject 转换为 JSON 字符串
    QByteArray toJsonByteArray(const QJsonObject &jsonObj);
private:
    mqtt_client_t *m_client;
    const char *m_topic;
    QTimer *m_timer;
    double temp;
    double hum;

public slots:
    void mqttPublishJson();
};



#endif // DATASENDTHREAD_H
