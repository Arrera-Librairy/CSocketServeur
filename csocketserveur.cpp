//Developpez par Enzo Bourkoua
//Le 10.12.2024
//Classe qui fonctionne de paire avec la classe CSocketClient. Comme le nom l'indique C'EST UN SOCKET

#include "csocketserveur.h"
#include <iostream>

CSocketServeur::CSocketServeur(QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &CSocketServeur::onNewConnection);
}

void CSocketServeur::startServer(int port) {
    if (!server->listen(QHostAddress::Any, port)) {
        std::cout << "Server could not start!" << std::endl;
    } else {
        std::cout << "Server started!"<< std::endl;
    }
}

void CSocketServeur::onNewConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &CSocketServeur::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    std::cout << "New client connected"<< std::endl;
}

void CSocketServeur::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QString out = QString(clientSocket->readAll());
        std::cout << "Server received:" << out.toStdString() << std::endl;
    }
}
