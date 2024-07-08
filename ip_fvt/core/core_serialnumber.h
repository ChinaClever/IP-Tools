#ifndef CORE_SERIALNUMBER_H
#define CORE_SERIALNUMBER_H

#include "core_http.h"
#include "cfgcom.h"
#include "json_pack.h"
class Core_Object : public QThread
{
    Q_OBJECT
public:
    explicit Core_Object(QObject *parent = nullptr);
    QString updateMacAddr(int step=1);
    QString createSn();

private:
    void wirteCfgMac();
    void writeMac(const QByteArray &mac);
    void initCurrentNum();
    void setCurrentNum();
    bool getDate();
    void setDate();

private:
    ushort mCurrentNum=0;
    sProgress *mPro;
};

#endif // CORE_SERIALNUMBER_H
