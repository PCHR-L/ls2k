#include "datasend.h"
#include "mymodbus.h"
#include <QDebug>

DataSender::DataSender(QObject *parent, MyModbus *modbus,mqtt_client_t *client, const char *topic)
    :QObject(parent), m_client(client), m_topic(topic), m_timer(new QTimer(this)),temp(0),hum(0)
{
    connect(modbus,&MyModbus::temperatureHumidityUpdated,this,&DataSender::setParameters);
}
DataSender::~DataSender()
{

}
void DataSender::setParameters(double wendu, double shidu)
{
    temp =wendu;
    hum = shidu;
}

// 创建 JSON 对象并填充数据
QJsonObject DataSender::createJsonObject(double temperature, double humidity) {
    QJsonObject jsonObj;
    jsonObj["temperature"] = temperature;
    jsonObj["humidity"] = humidity;
    return jsonObj;
}

// 将 QJsonObject 转换为 JSON 字符串
QByteArray  DataSender::toJsonByteArray(const QJsonObject &jsonObj) {
    QJsonDocument jsonDoc(jsonObj);
    return jsonDoc.toJson();
}

// MQTT 发布函数，现在使用 JSON 字符串作为有效载荷
void DataSender::mqttPublishJson() {
    QMutexLocker locker(&m_mutex); // 使用互斥锁保护临界区
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
        if(!mqtt_publish(m_client, m_topic, &msg))
            qDebug() << "消息发布失败";
    } else {
        qDebug() << "Invalid JSON format";
    }
}

void DataSender::publisuImage(QString imageurl)
{
    QMutexLocker locker(&m_mutex); // 使用互斥锁保护临界区
    QJsonObject jsonObj;
    jsonObj["ImageUrl"] = imageurl;
    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonBytes = jsonDoc.toJson();
    // 确保 JSON 字符串是有效的
    if (!QJsonDocument::fromJson(jsonBytes).isNull()) {
        // 构建 MQTT 消息
        mqtt_message_t msg;
        memset(&msg, 0, sizeof(msg));
        msg.payload = jsonBytes.data();
        msg.payloadlen = jsonBytes.size();
        msg.qos = QOS0; // 使用适当的服务质量
        // 发布消息
        if(!mqtt_publish(m_client, m_topic, &msg))
            qDebug() << "消息发布失败";
    } else {
        qDebug() << "Invalid JSON format";
    }
}
