/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "cfgcom.h"

CfgCom::CfgCom(const QString &fn, QObject *parent)  : CfgSerial(fn, parent)
{
    //openCfg(parent, fn);
    item = new sCfgComIt();
    initCfgCom();
    initErrData();
    item->vol = 250;
    item->coms.ser = nullptr;
    item->coms.sp = nullptr;
    item->coms.src = nullptr;
}

CfgCom *CfgCom::bulid(const QString& fn, QObject *parent)
{
    static CfgCom* sington = nullptr;
    if(!sington) {
        QString name = pathOfCfg(fn);
        sington = new CfgCom(name, parent);
    }
    return sington;
}

void CfgCom::initCfgCom()
{
    item->user = readCfg("user", "", "User").toString();
    item->pcNum = readCfg("pc_num", 0, "Sys").toInt();
}

void CfgCom::writeCfgCom()
{
    writeCfg("user", item->user, "User");
    writeCfg("pc_num", item->pcNum, "Sys");
}

void CfgCom::initErrData()
{
    item->volErr = readCfg("vol", 1,"Err").toInt();
    item->curErr = readCfg("cur", 1,"Err").toInt();
    item->powErr = readCfg("pow", 45,"Err").toInt();
}

void CfgCom::writeErrData()
{
    writeCfg("vol", item->volErr, "Err");
    writeCfg("cur", item->curErr, "Err");
    writeCfg("pow", item->powErr, "Err");
}
