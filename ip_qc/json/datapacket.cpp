/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "datapacket.h"

sDataPacket::sDataPacket()
{
    pro = new sProgress();
}

void sDataPacket::init()
{
    pro->step = 0;
    pro->result = 0;
    pro->pass.clear();
    pro->itPass.clear();
    pro->item.clear();
    pro->status.clear();
    pro->startTime = QTime::currentTime();

    pro->productType.clear();
    pro->productSN.clear();
    pro->macAddress.clear();
    pro->testTime.clear();

    pro->softwareType = "MPro-MonitorDebug";
    pro->companyName = "clever";
    pro->protocolVersion = "V1.0";
    // pro->testStartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    pro->testEndTime.clear();
    pro->no.clear();
    pro->itemName.clear();
    pro->uploadPass.clear();
    pro->pn.clear();
    pro->uploadPassResult = 0;
}


sDataPacket *sDataPacket::bulid()
{
    static sDataPacket* sington = nullptr;
    if(sington == nullptr)
        sington = new sDataPacket();
    return sington;
}


bool sDataPacket::delay(int s)
{
    bool ret = true;
    for(int i=0; i<s; ++i) {
        if((pro->step < Test_Over)  || (pro->step > Test_End)){
            QThread::msleep(100);
        } else {
            ret = false;
            break;
        }
    }

    return ret;
}
