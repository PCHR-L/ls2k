// httpserver.h
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>
#include <QUrl>


class HttpServer : public QObject {
    Q_OBJECT

public:
    HttpServer();
    ~HttpServer();

    void setParameters(double wendu, double shidu);

private slots:
    void onNewConnection();
    void readClientRequest();

private:
    QTcpServer *tcpServer;
    QMap<QTcpSocket*, QByteArray> requests;

    // 模拟的温湿度数据
    qreal temperature;
    qreal humidity;

};

#endif // HTTPSERVER_H
