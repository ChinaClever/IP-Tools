/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "rtu_write.h"

Rtu_Write::Rtu_Write(QObject *parent) : Rtu_Read(parent)
{

}

bool Rtu_Write::writes(sRtuSetItems &pkt)
{
    bool ret = false;
    uchar sendBuf[256]={0}, recvBuf[128]={0};
    int rtn = rtu_write_packets(&pkt, sendBuf);
    rtn = transmit(sendBuf, rtn, recvBuf, 2);
    if(rtn > 0) {
        ret = rtuRecvCrc(recvBuf, rtn);
        if(ret) {
            uchar fn = recvBuf[1];
            if(fn < 0x80) { // 设置正常
                ret = true;
            }
        }
    }
    return ret;
}

bool Rtu_Write::write(sRtuSetItem &pkt)
{
    bool ret = false;
    uchar sendBuf[256]={0}, recvBuf[128]={0};
    int rtn = rtu_write_packet(&pkt, sendBuf);
    rtn = transmit(sendBuf, rtn, recvBuf, 2);
    if(rtn > 0) {
        ret = rtuRecvCrc(recvBuf, rtn);
        if(ret) {
            uchar fn = recvBuf[1];
            if(fn < 0x80) { // 设置正常
                ret = true;
            }
        }
    }
    return ret;
}

int Rtu_Write::rtu_write_packet(sRtuSetItem *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    *(ptr++) = ((pkt->data) >> 8);
    *(ptr++) = (0xff)&(pkt->data);

    /*填入CRC*/
    pkt->crc = rtu_crc(buf, 6);
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    return 8;
}

int Rtu_Write::rtu_write_packets(sRtuSetItems *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    *(ptr++) = ((pkt->num) >> 8); /*长度高8位*/
    *(ptr++) = (0xff)&(pkt->num); /*低8位*/
    *(ptr++) = pkt->len;

    for(int i=0; i<pkt->len; ++i) {
        *(ptr++) = pkt->data[i];
    }

    /*填入CRC*/
    pkt->crc = rtu_crc(buf, 7+pkt->len);
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    return 9 + pkt->len;
}
