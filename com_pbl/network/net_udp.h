#ifndef NET_UDP_H
#define NET_UDP_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QtNetwork>

class Net_Udp : public QObject
{
    Q_OBJECT
public:
    explicit Net_Udp(QObject *parent = nullptr);
    bool writeDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);
    bool bind(int port, const QHostAddress &address = QHostAddress::Any);
    bool broadcastDatagram(const QByteArray &datagram, quint16 port);
    void close() {udpSocket->close();}

signals:
    void recvSig(const QString &ip, const QByteArray &array);

private slots:
    void recvSlot();

private:
    QByteArray readDatagram(QHostAddress *host = nullptr);

private:
    QUdpSocket *udpSocket = nullptr;
};

#endif // NET_UDP_H
