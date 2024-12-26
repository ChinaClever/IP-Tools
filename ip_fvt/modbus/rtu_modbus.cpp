/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "rtu_modbus.h"

Rtu_Modbus::Rtu_Modbus(QObject *parent) : QObject(parent)
{
    mRtuRw = new RtuRw(this);
}

Rtu_Modbus *Rtu_Modbus::bulid(QObject *parent)
{
    static Rtu_Modbus* sington = nullptr;
    if(sington == nullptr) {
        sington = new Rtu_Modbus(parent);
    }
    return sington;
}

RtuRw *Rtu_Modbus::get()
{
    // RtuRw *res = mRtuRw;
    // SerialPort *ser = CfgCom::bulid()->item->coms.src;
    // if(ser) {
    //     res->init(ser);
    //     //ser->reflush();
    // } else {
    //     res = nullptr;
    // }

    // return res;
}
