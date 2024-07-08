#ifndef NET_TCPCLIENT_H
#define NET_TCPCLIENT_H

#include "net_udp.h"
#include <QDataStream>
#include <QTcpSocket>

class Net_TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit Net_TcpClient(QObject *parent = nullptr);
    bool connectToHost(const QHostAddress &address, quint16 port);
    bool write(const QByteArray &data);
    QByteArray read();

private slots:
    void displayError(QAbstractSocket::SocketError socketError);

private:
     QTcpSocket *tcpSocket = nullptr;
};

#endif // NET_TCPCLIENT_H
