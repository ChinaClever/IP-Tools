#ifndef SSDP_SETTING_H
#define SSDP_SETTING_H

#include "ssdp_search.h"


struct sDataItem
{
    sDataItem():addr(0),rw(0),value(0){}
    uchar addr; // 地址
    uchar type; // 1 相数据  2 回路数据 ３　输出位数据 4组数据 6 环境 7 传感器
    uchar topic; // 1 开关  2 电压  3 电流  4 功率  11温度 12湿度
    uchar subtopic;  // 0 Size 1 当前值 2 额定值 3 报警状态  11 多开关控制
    uchar txType; // 通讯类型 1 UDP  3:SNMP  4：Zebra
    uchar id; // 0 表示统一设置
    uchar rw; // 0 读  1 写
    double value;
};

struct sCfgItem {
    sCfgItem():addr(0),id(0){}
    uchar txType; // 通讯类型 1 UDP  3:SNMP  4：Zebra
    uchar addr; // 地址
    uchar type; // 10 输出位  11 UUT信息
    uchar fc; // 功能码　0 表示统一设置
    uchar id;
};

class Ssdp_Setting : public Ssdp_Search
{
    Q_OBJECT
public:
    explicit Ssdp_Setting(QObject *parent = nullptr);
    bool setting(const sDataItem &it, const QString &room, const QString &ip);
    bool setCfg(const sCfgItem &it, const QVariant &v, const QString &room, const QString &ip);
    bool setJson(const QByteArray &data, const QString &room, const QString &ip);
    bool swVersion(const QString &room="", const QString &ip="");

signals:
    void sendDataSig(const QString &msg);
    void sendMsgSig(const QString &msg);
};

#endif // SSDP_SETTING_H
