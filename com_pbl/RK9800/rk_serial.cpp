#include "rk_serial.h"

Rk_Serial::Rk_Serial(QObject *parent) : QThread(parent)
{
    mSerial = nullptr;
}

int Rk_Serial::transmit(uchar *recv)
{
    if(!mSerial) return 0;
    static uchar sent[] = {0x55, 0xAA, 0x00, 0x01};
    int len = mSerial->transmit(sent, sizeof(sent), recv, 100);
    if(len < 10) { qDebug() << " Rk_Serial read len err" << len; return 0; }
    else if(memcmp(sent, recv, 3)) { qDebug() << " Rk_Serial read Data err" << len; return 0; }
    return len;
}

QString Rk_Serial::rkType(int id)
{
    QString str;
    switch (id) {
    case 0: str = "RK9800N"; break;
    case 1: str = "RK9901N"; break;
    case 2: str = "RK9813N"; break;
    case 3: str = "RK9940N"; break;
    case 4: str = "RK9980N"; break;
    }
    return str;
}

bool Rk_Serial::readPacket(sRkItem &it)
{
    bool ret = true;
    static uchar recv[2048] = {0};
    int len = transmit(recv);
    if(len > 0) {
        QByteArray array((char *)recv, len);
        QDataStream rxStream(array);
        rxStream.setByteOrder(QDataStream::LittleEndian);
        rxStream >> it.header >> it.flag >> it.rkId >> it.enAlarm
                >> it.vol >> it.curUnit >> it.cur >> it.pow
                >> it.ele >> it.hz >> it.pf >> it.crc;
        it.rkStr = rkType(it.rkId);
    } else ret = false;

    return ret;
}

bool Rk_Serial::clearEle()
{
    bool ret = true;
    static uchar sent[] = {0x55, 0xAA, 0x03, 0xFE};
    if(mSerial) {
        mSerial->write(sent, sizeof(sent));
    } else {
        ret = false;
    }

    return ret;
}
