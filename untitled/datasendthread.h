#ifndef DATASENDTHREAD_H
#define DATASENDTHREAD_H

#include <QThread>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "mqttclient.h"

class DataSenderThread : public QThread
{
    Q_OBJECT
public:
    DataSenderThread(QObject *parent = nullptr,mqtt_client_t *client = nullptr, const char *topic = nullptr);
    ~DataSenderThread();
    void run() override;
    void setParameters(double wendu, double shidu);
protected:
    static void topic1_handler(void* client, message_data_t* msg);
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
