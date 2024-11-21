#ifndef CFGCOM_H
#define CFGCOM_H
#include "cfgserial.h"

struct sCfgComIt
{
    QString user;
    sSerial coms;
    int logCount;
    uchar pcNum;
    ushort vol;
    ushort volErr, curErr, powErr; // 电流误差
};

class CfgCom : public CfgSerial
{
    CfgCom(const QString& fn, QObject *parent);
public:
    static CfgCom* bulid(const QString& fn=CFG_FN, QObject *parent = nullptr);
    sCfgComIt *item;
    void writeCfgCom();
    void writeErrData();

private:
    void initCfgCom();
    void initErrData();
};

#endif // CFGCOM_H
