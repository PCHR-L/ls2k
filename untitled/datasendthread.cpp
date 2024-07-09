#include "datasendthread.h"
#include <QDebug>

DataSenderThread::DataSenderThread(QObject *parent, mqtt_client_t *client, const char *topic)
    :QThread(parent), m_client(client), m_topic(topic), m_timer(new QTimer(this)),temp(0),hum(0)
{
    connect(m_timer, &QTimer::timeout, this, &DataSenderThread::mqttPublishJson);
}
DataSenderThread::~DataSenderThread()
{
}
void DataSenderThread::setParameters(double wendu, double shidu)
{
    temp =wendu;
    hum = shidu;
}

// 创建 JSON 对象并填充数据
QJsonObject DataSenderThread::createJsonObject(double temperature, double humidity) {
    QJsonObject jsonObj;
    jsonObj["temperature"] = temperature;
    jsonObj["humidity"] = humidity;
    return jsonObj;
}

// 将 QJsonObject 转换为 JSON 字符串
QByteArray  DataSenderThread::toJsonByteArray(const QJsonObject &jsonObj) {
    QJsonDocument jsonDoc(jsonObj);
    return jsonDoc.toJson();
}

// MQTT 发布函数，现在使用 JSON 字符串作为有效载荷
void DataSenderThread::mqttPublishJson() {
    QJsonObject jsonObj = createJsonObject(temp,hum);
    QByteArray jsonBytes = toJsonByteArray(jsonObj);

    // 确保 JSON 字符串是有效的
    if (!QJsonDocument::fromJson(jsonBytes).isNull()) {
        // 构建 MQTT 消息
        mqtt_message_t msg;
        memset(&msg, 0, sizeof(msg));
        msg.payload = jsonBytes.data();
        msg.payloadlen = jsonBytes.size();
        msg.qos = QOS0; // 使用适当的服务质量

        // 发布消息
        mqtt_publish(m_client, m_topic, &msg);
    } else {
        qDebug() << "Invalid JSON format";
    }
}

void DataSenderThread::run()
{
    // 启动定时器，设置间隔为 10000 毫秒（10 秒）
    m_timer->start(10000);
    // 进入事件循环，等待定时器触发
    exec();
}

