#ifndef SSDP_OBJ_H
#define SSDP_OBJ_H

#include "net_udp.h"
#include "print.h"

#define START_HEAD ((ushort)0xC5C5)
#define END_CRC ((ushort)0x5C5C)

struct sSdpIt{
    sSdpIt():version(1){}
    int fc;
    int version;
    QString room;
    QString ip;
    QString target;
    QByteArray data;
    ushort crc;
};

class Ssdp_Obj : public QObject
{
    Q_OBJECT
public:
    explicit Ssdp_Obj(QObject *parent = nullptr);
    bool udpSend(const QHostAddress &host, const sSdpIt &it);
    bool ssdpSend(const sSdpIt &it);

signals:
    void recvSig(const sSdpIt &it);

protected:
    bool udpBind();
    bool ssdpBind();

private:
    void recvMsg(const QString &ip, const QByteArray &array);
    QByteArray toArray(const sSdpIt &it);
    bool toItem(const QByteArray &array, sSdpIt &it);

protected slots:
    virtual void readMsgSlot();
    void recvUdpSlot(const QString &ip, const QByteArray &array) {recvMsg(ip, array);}
    virtual bool rplySearchTarget(const sSdpIt &){return false;}

private:
    int mPort;
    Net_Udp *mUdp;
    QUdpSocket *mSocket;
    QHostAddress mAddress;
};

#endif // SSDP_OBJ_H
