#include "httpserver.h"
#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>


HttpServer::HttpServer() {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &HttpServer::onNewConnection);
    if (!tcpServer->listen(QHostAddress::Any, 8080)) {
        qDebug("Server could not start!");
    }
}

HttpServer::~HttpServer() {
    delete tcpServer;
}

void HttpServer::onNewConnection() {
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    if (clientSocket) {
        connect(clientSocket, &QTcpSocket::readyRead, this, &HttpServer::readClientRequest);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

void HttpServer::readClientRequest() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        return;
    }

    QByteArray requestData = clientSocket->readAll();
    QString requestString = QString(requestData);
    QString response;

    if (requestString.contains("GET / ") || requestString.contains("GET /index.html")) {
        // 发送HTML文件
        QFile file("index.html");
        if (file.open(QIODevice::ReadOnly)) {
            response = file.readAll();
            response.prepend("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        }
    } else if (requestString.contains("/temperaturehumidity")) {
        // 发送JSON数据

        QJsonObject json;
        json["temperature"] = temperature;
        json["humidity"] = humidity;

        QJsonDocument doc(json);
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "\r\n"
                   + doc.toJson(QJsonDocument::Compact) +
                   QStringLiteral("\r\n\r\n");
    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    clientSocket->write(response.toUtf8());
    clientSocket->flush();
    clientSocket->disconnectFromHost();
}


void HttpServer::setParameters(double wendu, double shidu)
{
    temperature =wendu;
    humidity = shidu;
}
