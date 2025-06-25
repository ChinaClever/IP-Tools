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

    uint devSpec; // 设备规格 0-互感器 1-32A小锰铜 2-直流
    uint language; // 0 中文 1 英文
    uint isBreaker; // 0没有断路器 1有断路器
    uint vh; // 0:垂直 1:水平
    uint standNeutral; // 0-标准,1-中性
    uint webBackground; // 网页背景颜色
    uint oldProtocol; //兼容旧协议 0-标准 1-兼容旧协议 2-华为ECC

    uint lineNum; //设备单三相
    uint loopNum; // 回路数量
    QString devType; // 设备类型
    QString fwVer; //软件版本号

    bool isEle;
    bool isTimer;
    bool isMac;
    bool isSersor;
    bool isLink;
    bool snPrint;
    bool macPrint;
    bool fwPrint;

    double cpuTem;

    double lineVol;
    double lineCur;
    double linePow;

    double loopVol;
    double loopCur;
    double loopPow;
};

class CfgCom : public CfgSerial
{
    CfgCom(const QString& fn, QObject *parent);
public:
    static CfgCom* bulid(const QString& fn=CFG_FN, QObject *parent = nullptr);
    sCfgComIt *item;
    void writeCfgCom();
    void writeErrData();
    void writeParams();
    void writeThresholds();

private:
    void initCfgCom();
    void initErrData();
    void initCfgData();
};

#endif // CFGCOM_H
