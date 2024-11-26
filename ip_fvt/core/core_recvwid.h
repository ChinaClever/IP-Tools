#ifndef CORE_RECVWID_H
#define CORE_RECVWID_H

#include <QUdpSocket>
#include <QHostAddress>
#include <QtCore>

class Core_RecvWid: public QObject
{    Q_OBJECT

public:
    static Core_RecvWid *bulid(QObject *parent = nullptr);
    explicit Core_RecvWid(QObject *parent = nullptr);
    QString Send_Recv();

private slots:
    void udpRecvSlot();

private:
    QUdpSocket *udpSocket;
    QString data;
};


#endif // CORE_RECVWID_H
