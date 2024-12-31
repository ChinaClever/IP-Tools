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
    initCfgData();
    item->coms.ser = nullptr;
    item->coms.sp = nullptr;
    item->coms.src = nullptr;
    item->vol = 250;

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

void CfgCom::initCfgData()
{
    item->lineNum = readCfg("lineNum", 1,"Params").toInt();
    item->vh = readCfg("vh", 1,"Params").toInt();
    item->standNeutral = readCfg("standNeutral", 0,"Params").toInt();
    item->devSpec = readCfg("devSpec", 1,"Params").toInt();
    item->oldProtocol = readCfg("oldProtocol", 1,"Params").toInt();
    item->isBreaker = readCfg("isBreaker", 1,"Params").toInt();
    item->language = readCfg("language", 0,"Params").toInt();
    item->loopNum = readCfg("loopNum", 1,"Params").toInt();
    item->devType = readCfg("devType", "","Params").toString();
    item->fwVer = readCfg("fwVer", "","Params").toString();
    item->isTimer = readCfg("isTimer", "","Params").toBool();
    item->isMac = readCfg("isMac", "","Params").toBool();
    item->isSersor = readCfg("isSersor", "","Params").toBool();
    item->isLink = readCfg("isLink", "","Params").toBool();

    item->lineVol = readCfg("lineVol", 1,"Thresholds").toDouble();
    item->lineCur = readCfg("lineCur", 1,"Thresholds").toDouble();
    item->linePow = readCfg("linePow", 0,"Thresholds").toDouble();
    item->loopVol = readCfg("loopVol", 1,"Thresholds").toDouble();
    item->loopCur = readCfg("loopCur", 1,"Thresholds").toDouble();
    item->loopPow = readCfg("loopPow", 0,"Thresholds").toDouble();

}
void CfgCom::writeParams()
{
    writeCfg("lineNum", item->lineNum, "Params");
    writeCfg("vh", item->vh, "Params");
    writeCfg("standNeutral", item->standNeutral, "Params");
    writeCfg("devSpec", item->devSpec, "Params");
    writeCfg("oldProtocol", item->oldProtocol, "Params");
    writeCfg("isBreaker", item->isBreaker, "Params");
    writeCfg("language", item->language, "Params");
    writeCfg("loopNum", item->loopNum, "Params");
    writeCfg("devType", item->devType, "Params");
    writeCfg("fwVer", item->fwVer, "Params");
    writeCfg("isTimer", item->isTimer, "Params");
    writeCfg("isMac", item->isMac, "Params");
    writeCfg("isSersor", item->isSersor, "Params");
    writeCfg("isLink", item->isLink, "Params");
}

void CfgCom::writeThresholds()
{
    writeCfg("lineVol", item->lineVol, "Thresholds");
    writeCfg("lineCur", item->lineCur, "Thresholds");
    writeCfg("linePow", item->linePow, "Thresholds");
    writeCfg("loopVol", item->loopVol, "Thresholds");
    writeCfg("loopCur", item->loopCur, "Thresholds");
    writeCfg("loopPow", item->loopPow, "Thresholds");
}
