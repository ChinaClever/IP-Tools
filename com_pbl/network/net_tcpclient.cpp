/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "net_tcpclient.h"

Net_TcpClient::Net_TcpClient(QObject *parent)
    : QObject{parent} , tcpSocket(new QTcpSocket(this))
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &Net_TcpClient::displayError);
#endif
}

bool Net_TcpClient::connectToHost(const QHostAddress &address, quint16 port)
{
    tcpSocket->abort();
    tcpSocket->connectToHost(address, port);
    bool ret = tcpSocket->waitForConnected();
    if(!ret) qCritical() << tcpSocket->errorString();
    return ret;
}

QByteArray Net_TcpClient::read()
{
    return tcpSocket->readAll();
}

bool Net_TcpClient::write(const QByteArray &data)
{
    int ret = tcpSocket->write(data);
    if(ret < 0){ret=0; qCritical() << tcpSocket->errorString();}
    return ret;
}

void Net_TcpClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qCritical() << tr("The host was not found. Please check the "
                          "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qCritical() << tr("The connection was refused by the peer. "
                          "Make sure the fortune server is running, "
                          "and check that the host name and port "
                          "settings are correct.");
        break;
    default:
        qCritical() << tr("The following error occurred: %1.")
                       .arg(tcpSocket->errorString());
    }

}
