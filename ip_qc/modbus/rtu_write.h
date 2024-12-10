#ifndef RTU_WRITE_H
#define RTU_WRITE_H

#include "rtu_read.h"

struct sRtuSetItem
{
    uchar addr;
    uchar fn;
    ushort reg;
    ushort data;
    ushort crc;
};

struct sRtuSetItems
{
    uchar addr;
    uchar fn;
    ushort reg;
    ushort num;
    uchar len;
    uchar data[MODBUS_RTU_SIZE];
    ushort crc;
};

struct sRtuSetReplyItem
{
    uchar addr;
    uchar fn;
    uchar err;
    ushort crc;
};

class Rtu_Write : public Rtu_Read
{
    Q_OBJECT
public:
    explicit Rtu_Write(QObject *parent = nullptr);

    bool write(sRtuSetItem &pkt);
    bool writes(sRtuSetItems &pkt);

protected:
    int rtu_write_packet(sRtuSetItem *pkt, uchar *ptr);
    int rtu_write_packets(sRtuSetItems *pkt, uchar *ptr);
};

typedef Rtu_Write RtuRw;

#endif // RTU_WRITE_H
