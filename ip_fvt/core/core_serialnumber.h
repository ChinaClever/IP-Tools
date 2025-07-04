#ifndef CORE_SERIALNUMBER_H
#define CORE_SERIALNUMBER_H

#include "core_http.h"
#include "cfgcom.h"
#include "json_pack.h"

#define LOOP_NUM 9
#define LINE_NUM 3

struct sParameter {
    uint language; // 0 中文 1 英文
    uint isBreaker; // 0没有断路器 1有断路器
    uint sensorBoxEn; // 传感器盒子 0：禁用， 1：启用
    uint vh; // 0:垂直 1:水平
    uint standNeutral; // 0-标准,1-中性
    uint webBackground; // 网页背景颜色
    uint supplyVol;

    uint lineNum; //设备单三相
    uint loopNum; // 回路数量
};

struct sThreshold
{
    double lineVol[LINE_NUM];
    double lineCur[LINE_NUM];
    double linePow[LINE_NUM];
    double pf[LINE_NUM];
    double ele[LINE_NUM];

    double loopVol[LOOP_NUM];
    double loopCur[LOOP_NUM];
    double loopPow[LOOP_NUM];
    double volValue; //电压参考值
};

struct sMonitorData
{
    double apparent_pow;
    double tg_ele;
    double tg_pow;
    QVariantList temps;
    QVariantList doors;
};

struct sCoreUnit
{
    sParameter param;
    sThreshold value;
    sMonitorData data;
};


struct sCoreItem
{
    int port = 3166;
    QString ip="192.168.1.163";
    QString logo="logo.png";
    sCoreUnit desire; // 期望
    sCoreUnit actual; // 实际
    QString jsonPacket;
    QString datetime;
    QString mac,sn;
    QVariantList mcutemp;
    int alarm;
};

class Core_Object : public QThread
{
    Q_OBJECT
public:
    explicit Core_Object(QObject *parent = nullptr);
    static sCoreItem coreItem;
    QString updateMacAddr(int step=1);
    QString createSn();
    bool jsonAnalysis();

private:
    void wirteCfgMac();
    void writeMac(const QByteArray &mac);
    void initCurrentNum();
    void setCurrentNum();
    bool getDate();
    void setDate();

    void getSn(const QJsonObject &object);
    void getMac(const QJsonObject &object);
    void getTgData(const QJsonObject &object);
    void getEnvData(const QJsonObject &object);
    void getParameter(const QJsonObject &object);
    void getPduData(const QJsonObject &object);
    void getAlarmStatus(const QJsonObject &object);
    double getActualValue(const QJsonObject &object, const QString &key, int value, const QString &suffix="value");
    double getRating(const QJsonObject &object, const QString &key, const QString &suffix="rated");

    bool checkInput(const QByteArray &msg, QJsonObject &obj);
    double getData(const QJsonObject &object, const QString &key);
    QJsonValue getValue(const QJsonObject &object, const QString &key);
    QJsonArray getArray(const QJsonObject &object, const QString &key);
    QJsonObject getObject(const QJsonObject &object, const QString &key);

private:
    ushort mCurrentNum=0;
    sProgress *mPro;
};

#endif // CORE_SERIALNUMBER_H
