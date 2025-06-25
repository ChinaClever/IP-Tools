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
    obj->lineVol.clear(); obj->linePF.clear();
    obj->lineHz.clear(); sRkItem rkIt;

    bool ret = mRk->readPacket(rkIt);
    if(ret) {
        for(int i=0; i<3; ++i) {
            obj->lineVol << rkIt.vol/10.0;
            obj->lineCur << rkIt.cur / 1000.0;
            obj->linePow << rkIt.pow / 1000000.0;
            obj->linePF << rkIt.pf / 1000.0;
            obj->lineHz << rkIt.hz / 100.0;
        }
    } //cout << obj->lineVol << obj->lineCur <<  obj->linePow << obj->pf << obj->hz;

    return ret;
}


