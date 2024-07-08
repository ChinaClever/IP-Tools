#ifndef CFGSERIAL_H
#define CFGSERIAL_H

#include "cfg_app.h"
#include "serialstatuswid.h"

struct sSerial
{
    sSerial() {sp=src=ser=nullptr;}
    SerialPort *sp; // 标准源
    SerialPort *src; // 串口对象
    SerialPort *ser; // 串口对象
};

class CfgSerial : public Cfg_Obj
{
public:
    explicit CfgSerial(const QString& fn, QObject *parent = nullptr);

    QString getSerialBr(const QString &com);
    QString getSerialName(const QString &key);
    void setSerialBr(const QString &com, const QString &br);
    void setSerialName(const QString &key, const QString &v);

    QString getLoginName();
    void setLoginName(const QString &name);
};

#endif // CFGSERIAL_H
