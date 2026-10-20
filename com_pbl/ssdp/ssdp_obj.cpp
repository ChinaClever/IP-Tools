/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include <QtGlobal>

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "ssdp_obj.h"
#include <QNetworkInterface>

namespace {

QList<QHostAddress> localIpv4List()
{
    QList<QHostAddress> ips;
    const auto ifaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : ifaces) {
        const auto flags = iface.flags();
        if (!(flags & QNetworkInterface::IsUp) || !(flags & QNetworkInterface::IsRunning))
            continue;
        if (flags & QNetworkInterface::IsLoopBack)
            continue;

        const auto entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            const QHostAddress ip = entry.ip();
            if (ip.protocol() == QAbstractSocket::IPv4Protocol && !ip.isLoopback())
                ips << ip;
        }
    }
    return ips;
}

bool setMulticastInterface(QUdpSocket *sock, const QHostAddress &local)
{
    const qintptr fd = sock->socketDescriptor();
    if (fd < 0)
        return false;

    const quint32 addr = htonl(local.toIPv4Address());
#ifdef Q_OS_WIN
    const int rc = ::setsockopt(static_cast<SOCKET>(fd), IPPROTO_IP, IP_MULTICAST_IF,
                                 reinterpret_cast<const char *>(&addr), sizeof(addr));
#else
    const int rc = ::setsockopt(static_cast<int>(fd), IPPROTO_IP, IP_MULTICAST_IF,
                                 &addr, sizeof(addr));
#endif
    return rc == 0;
}

}

Ssdp_Obj::Ssdp_Obj(QObject *parent) : QObject(parent)
{
    mPort = 9123;
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
    QByteArray array = toArray(it);
    if (mSocket->state() != QAbstractSocket::BoundState) {
        mSocket->bind(QHostAddress::AnyIPv4, 0,
                      QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    }

    // 多网卡时系统只从默认路由网卡发组播，PDU 在其它网卡上就搜不到
    const QList<QHostAddress> locals = localIpv4List();
    if (locals.isEmpty()) {
        const qint64 rcv = mSocket->writeDatagram(array, mAddress, mPort);
        mSocket->flush();
        if (rcv < 0) {
            cout << "Error: SSDP write" << mSocket->errorString();
            return false;
        }
        return true;
    }

    bool ret = false;
    for (const QHostAddress &local : locals) {
        if (!setMulticastInterface(mSocket, local))
            continue;
        const qint64 rcv = mSocket->writeDatagram(array, mAddress, mPort);
        mSocket->flush();
        if (rcv >= 0)
            ret = true;
        else
            cout << "Error: SSDP write" << local.toString() << mSocket->errorString();
    }
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

