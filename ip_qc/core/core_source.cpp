/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "core_source.h"

Core_Source::Core_Source(QObject *parent) : Core_Object(parent)
{
    mRk = new Rk_Serial(this);
    QTimer::singleShot(850,this,SLOT(initFunSlot()));
}


void Core_Source::initFunSlot()
{
    sCfgComIt *it = CfgCom::bulid()->item;
    if(it->coms.src) {
        it->coms.src->setBaudRate(4800);
        mRk->init(it->coms.src);
    } else QTimer::singleShot(850,this,SLOT(initFunSlot()));
}

bool Core_Source::readRk9901()
{
    sPdudata *obj = &coreItem.desire.value;
    obj->lineCur.clear(); obj->linePow.clear();
    obj->lineVol.clear(); sRkItem rkIt;

    bool ret = mRk->readPacket(rkIt);
    if(ret) {
        int curUnit = 100;
        if(rkIt.curUnit) curUnit *= 10;
        for(int i=0; i<3; ++i) {
            obj->lineVol << rkIt.vol;
            obj->lineCur << rkIt.cur / curUnit;
            obj->linePow << rkIt.pow / 1000;
            //obj->hz[i] = rkIt.hz / 100;
            //obj->pf[i] = rkIt.pf / 10;
        }
    }

    return ret;
}


