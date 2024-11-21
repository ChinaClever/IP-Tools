#ifndef OBJECTTHREAD_H
#define OBJECTTHREAD_H

#include "datapacket.h"
#include "rtu_modbus.h"

class BaseObject
{
public:
    BaseObject();
    void init() {mPacket->init();}

protected:
    sDevType *mDt;
    sTypeCfg *mCfg;
    sDevData *mDev;
    RtuRw *mModbus;
    sCfgComIt *mItem;
    sProgress *mPro;
    sObjData *mData;
    sDataPacket *mPacket;
};


class BaseThread : public QThread, public BaseObject
{
    Q_OBJECT
public:
    explicit BaseThread(QObject *parent = nullptr);
    ~BaseThread();

    bool updatePro(const QString &str, bool pass=true, int sec=0);
    bool updateValuePro(const QString &str, bool pass=true, int sec=0);
    bool delay(int s=1) {return mdelay(6*s);}
    bool mdelay(int s=1);

protected slots:
    virtual void initFunSlot();

protected:
    bool isRun;

};

#endif // OBJECTTHREAD_H
