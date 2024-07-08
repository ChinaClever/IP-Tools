/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include "ssdp_obj.h"

Ssdp_Obj::Ssdp_Obj(QObject *parent) : QObject(parent)
{
    mPort = 9126;
    mUdp = new Net_Udp(this);
    mSocket = new QUdpSocket(this);
    qRegisterMetaType<sSdpIt>("sSdpIt");
    mAddress = QHostAddress("239.255.43.21");
}

bool Ssdp_Obj::ssdpBind()
{
    auto ok = mSocket->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::DontShareAddress);
    if(ok) ok = mSocket->joinMulticastGroup(mAddress);
    if(ok) connect(mSocket,SIGNAL(readyRead()),this,SLOT(readMsgSlot()));
    else cout << mSocket->errorString();
    return ok;
}

bool Ssdp_Obj::udpBind()
{
    bool ret = mUdp->bind(mPort+1);
    if(ret) connect(mUdp,&Net_Udp::recvSig,this,&Ssdp_Obj::recvUdpSlot);
    return ret;
}

void Ssdp_Obj::readMsgSlot()
{
    QByteArray reply; QHostAddress host;
    while (mSocket->hasPendingDatagrams()) {
        reply.resize(mSocket->pendingDatagramSize());
        mSocket->readDatagram(reply.data(), reply.size(), &host);
        recvMsg(host.toString(), reply);  cout << host.toString();
    }
}

QByteArray Ssdp_Obj::toArray(const sSdpIt &it)
{
    QByteArray array; QDataStream in(&array, QIODevice::WriteOnly);
    in << it.version << it.fc <<  it.room << it.ip << it.target << it.data << END_CRC;
    return qCompress(array);
}

bool Ssdp_Obj::toItem(const QByteArray &array, sSdpIt &it)
{
    QByteArray data = qUncompress(array);
    bool ret = false; if(data.isEmpty()) return ret;
    QDataStream out(&data, QIODevice::ReadOnly);
    out >> it.version >> it.fc >> it.room >> it.ip >> it.target >> it.data >> it.crc;
    if(it.crc == END_CRC) ret = true;
    return ret;
}

bool Ssdp_Obj::ssdpSend(const sSdpIt &it)
{
    bool ret = true; QByteArray array = toArray(it); //cout << array.size();
    auto rcv = mSocket->writeDatagram(array, mAddress, mPort); mSocket->flush();
    if(rcv < 0) { ret = false; cout << "Error: SSDP write" << mSocket->errorString();}
    return ret;
}

bool Ssdp_Obj::udpSend(const QHostAddress &host, const sSdpIt &it)
{
    QByteArray array = toArray(it);
    return mUdp->writeDatagram(array, host, mPort+1);
}

void Ssdp_Obj::recvMsg(const QString &ip, const QByteArray &array)
{
    sSdpIt it; if(toItem(array, it)) {
        if(!ip.contains(it.ip)) {it.ip = ip; it.ip.remove("::ffff:");}
        emit recvSig(it); //cout << ip;
    } else cout << it.fc << it.crc << array.size();
}

