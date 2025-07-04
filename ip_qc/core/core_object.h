#ifndef CORE_OBJECT_H
#define CORE_OBJECT_H

#include "core_http.h"
// #include "cfgcom.h"
#include "modbus/rtu_modbus.h"

#define LOOP_NUM 9
#define LINE_NUM 3

struct sParameter {
    uint devSpec; // 设备规格 0-互感器 1-32A小锰铜 2-直流
    uint language; // 0 中文 1 英文
    uint isBreaker; // 0没有断路器 1有断路器
    uint oldProtocol; //兼容旧协议
    uint vh; // 0:垂直 1:水平
    uint standNeutral; // 0-标准,1-中性
    uint webBackground; // 网页背景颜色

    uint lineNum; //设备单三相
    uint loopNum; // 回路数量
    double cpuTem;
};

struct sPdudata
{
    QVariantList lineVol;
    QVariantList lineCur;
    QVariantList linePow;
    QVariantList linePF;
    QVariantList lineHz;
    QVariantList lineEle;

    QVariantList loopVol;
    QVariantList loopCur;
    QVariantList loopPow;
    QVariantList loopPF;
    QVariantList loopHz;
    QVariantList loopEle;
};

struct sThreshold
{
    QVariantList lineVol;
    QVariantList lineCur;
    QVariantList linePow;

    QVariantList loopVol;
    QVariantList loopCur;
    QVariantList loopPow;

    double volValue; //电压参考值
};

struct mThreshold
{
    double lineVol;
    double lineCur;
    double linePow;

    double loopVol;
    double loopCur;
    double loopPow;

    double volValue; //电压参考值
};

struct sMonitorData
{
    double apparent_pow;
    double reactive_pow;
    double active_pow;

    double tg_ele;
    double tg_reactiveEle;
    double tg_apparentEle;

    QVariantList temps;
    QVariantList doors;
};

struct sVersion
{
    QString devType; // 设备类型
    QString fwVer; //软件版本号
};

struct sCoreUnit
{
    sVersion ver;
    sParameter param;
    sThreshold rate;
    sPdudata value;
    sMonitorData data;
    QString datetime;
    QString mac,sn, uuid;
    // QVariantList mcutemp;
    int alarm;
};

struct mCoreUnit
{
    sVersion ver;
    sParameter param;
    mThreshold rate;
    sPdudata value;
    sMonitorData data;
    QString datetime;
    QString mac,sn, uuid;
    // QVariantList mcutemp;
    int alarm;
};

struct sCoreItem
{
    int port = 3166;
    QString ip="192.168.1.163";
    QString logo="logo.png";
    QString tlsFile;
    mCoreUnit desire; // 期望
    sCoreUnit actual; // 实际
    QString jsonPacket;
};



class Core_Object : public QThread
{
    Q_OBJECT
public:
    explicit Core_Object(QObject *parent = nullptr);
    static sCoreItem coreItem;
    void clearAllEle();
    void clearLogs();
    void enCascade(int v);
    void factoryRestore();
    void rtuSet(int v);
    void boxSet(int v);
    void relayCtrl(int on, int id=0);
    void relayDelay(int sec, int id=0);
    void readMetaData();
    void timeSync();
    void irqCheck();
    void setRunTime();
    bool jsonAnalysis();
    bool jsonAnalysisRefer();
    void setModbus();
    void reset();

private:
    void getSn(const QJsonObject &object);
    void getMac(const QJsonObject &object);
    void getTgData(const QJsonObject &object);
    void getEnvData(const QJsonObject &object);
    void getParameter(const QJsonObject &object);
    void getThreshold(const QJsonObject &object);
    void getAlarmStatus(const QJsonObject &object);
    void getPduData(const QJsonObject &object);
    void getDevType(const QJsonObject &object);

    void getTgDataRefer(const QJsonObject &object);
    void getEnvDataRefer(const QJsonObject &object);
    void getParameterRefer(const QJsonObject &object);
    void getPduDataRefer(const QJsonObject &object);

    double getRating(const QJsonObject &object, const QString &key,  int value, const QString &suffix="rated");
    double getActualValue(const QJsonObject &object, const QString &key, int value, const QString &suffix="value");


    bool checkInput(const QByteArray &msg, QJsonObject &obj);
    double getData(const QJsonObject &object, const QString &key);
    QJsonValue getValue(const QJsonObject &object, const QString &key);
    QJsonArray getArray(const QJsonObject &object, const QString &key);
    QJsonObject getObject(const QJsonObject &object, const QString &key);

protected:
    Core_Http *mHttp;
    RtuRw *mModbus;
};

#endif // CORE_OBJECT_H
