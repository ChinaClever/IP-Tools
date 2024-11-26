#include "core_recvwid.h"

Core_RecvWid::Core_RecvWid(QObject *parent)
    : QObject{parent}
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::AnyIPv4, 21907, QUdpSocket::ShareAddress);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Core_RecvWid::udpRecvSlot);
}

Core_RecvWid *Core_RecvWid::bulid(QObject *parent)
{
    static Core_RecvWid* sington = nullptr;
    if(!sington) sington = new Core_RecvWid(parent);
    return sington;
}

void Core_RecvWid::udpRecvSlot()
{
    qDebug()<<"udpRecvSlot";
    QByteArray datagram; QHostAddress host;
    while(udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        int ret = udpSocket->readDatagram(datagram.data(), datagram.size(), &host);
        if(ret > 0) { qDebug()<<"QString(datagram)"<<QString(datagram); data += QString(datagram) + ";";}
        else qCritical() << udpSocket->errorString();
    }
}

QString Core_RecvWid::Send_Recv()
{
    return data;
}
