#ifndef DATASENDTHREAD_H
#define DATASENDTHREAD_H

#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "mqttclient.h"
#include "mymodbus.h"

#include <QMutex>
#include <QMutexLocker>

class DataSender : public QObject
{
    Q_OBJECT
public:
    DataSender(QObject *parent = nullptr,MyModbus *modbus = nullptr,mqtt_client_t *client = nullptr, const char *topic = nullptr);
    ~DataSender();
    void setParameters(double wendu, double shidu);

    QString imageUrl;

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

    QMutex m_mutex; // 添加互斥锁

public slots:
    void mqttPublishJson();
    void publisuImage(QString imageurl);
};



#endif // DATASENDTHREAD_H
