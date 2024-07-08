/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "net_tcpserver.h"

Net_TcpServer::Net_TcpServer(QObject *parent)
    : QObject{parent}
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnection()));
}

bool Net_TcpServer::listen(int port)
{
    return tcpServer->listen(QHostAddress::Any, port);
}

void Net_TcpServer::onNewConnection()
{
    while(tcpServer->hasPendingConnections()){
        QTcpSocket *sock = tcpServer->nextPendingConnection();
        connect(sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(sock, SIGNAL(disconnected()), sock, SLOT(deleteLater()));
    }
}

void Net_TcpServer::onReadyRead()
{
    QTcpSocket *sock = tcpServer->nextPendingConnection();
    if(sock->bytesAvailable() > 0){
        QByteArray inData = sock->readAll();
        emit recvSig(inData); sock->flush();
    }
}
