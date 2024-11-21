/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "yc_standsource.h"

YC_StandSource::YC_StandSource(QObject *parent) : BaseThread(parent)
{
    mSerial = nullptr;
}

void YC_StandSource::initFunSlot()
{
    mSerial = mItem->coms.sp;
    if(mSerial) mSerial->setTs(2);
    else QTimer::singleShot(350,this,SLOT(initFunSlot()));
}

bool YC_StandSource::setBaudRate(qint32 baudRate)
{
    bool ret = false;
    if(mSerial) {
        ret = mSerial->setBaudRate(baudRate);
    }
    return ret;
}

bool YC_StandSource::write(QByteArray &array)
{
    bool ret = false;
    if(mSerial) {
        array.append(0x0D);
        ret = serialWrite(array);
    }

    return ret;
}


bool YC_StandSource::powerOn(int v)
{
    bool ret = setVol(220);
    if(ret) ret = setCur(v);

    return ret;
}

bool YC_StandSource::powerDown()
{
    setVol(0,0);
    setCur(0,0);
    return initFun();
}

bool YC_StandSource::powerReset()
{
    powerDown();
    bool ret = delay(6);
    if(ret) {
       ret = powerOn();
    }

    return ret;
}

