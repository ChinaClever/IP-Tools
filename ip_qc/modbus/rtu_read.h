#ifndef RTU_READ_H
#define RTU_READ_H

#include "rtu_serial.h"

#define MODBUS_RTU_SIZE 255

struct sRtuItem {
    sRtuItem():fn(3),reg(0),num(0){} // 下位机有问题
    uchar addr; // 表示从机地址码
    uchar fn;  // 表示功能码
    ushort reg; // 表示寄存器首地址
    ushort num; // 表示数据长度
    ushort crc; // 表示CRC校验
};

struct sRtuReplyItem {
    sRtuReplyItem():fn(3),len(0){} // 下位机有问题
    uchar addr; // 表示从机地址码
    uchar fn;  // 表示功能码
    uchar len; // 表示数据长度
    uchar data[MODBUS_RTU_SIZE]; // 表示寄存器首地址
    ushort crc; // 表示CRC校验
};


class Rtu_Read : public Rtu_Serial
{
    Q_OBJECT
public:
    explicit Rtu_Read(QObject *parent = nullptr);

    int read(sRtuItem &pkt, uchar *recv, int id=0);
    bool readCheck(sRtuItem *pkt, sRtuReplyItem *recv);

protected:
    ushort calccrc (ushort crc, uchar crcbuf);
    int rtuPacket(sRtuItem *pkt, uchar *ptr);
    int rtuRecvSIData(uchar *ptr,  sRtuReplyItem *pkt);
    int rtuRecvIPData(uchar *ptr,  sRtuReplyItem *pkt);

    bool rtuRecvCrc(uchar *buf, int len);
    bool recvCrc(uchar *buf, int len, sRtuReplyItem *msg);
    int rtuRead(sRtuItem *pkt, sRtuReplyItem *recv , int id);
};

#endif // RTU_READ_H
