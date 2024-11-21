#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_serialnumber.h"
#include "json_pack.h"
#include "yc_obj.h"

class Core_Thread : public Core_Object
{
    Q_OBJECT
public:
    explicit Core_Thread(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    QString m_mac, m_sn;
    QStringList getFs();
    void run();
    bool initFun();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);
    void overSig(); void startSig();

public slots:
    void startSlot() {run();}

protected:
    void timeSync();
    void enModbusRtu();
    bool startCalibration();
    bool startCheck();
    bool downVer(const QString &ip);
    bool workDown(const QString &ip);
    void writeSnMac(const QString &sn, const QString &mac);

    bool eachCurEnter(int exValue);
    bool eachCurCheck(int k, int exValue, bool res);
    void readMetaData();

    bool curRangeByID(int i, int exValue, int cnt, bool flag);
    bool powRangeByID(int i, int exValue, int cnt, bool flag);
    bool curErrRange(int exValue, int cur);
    bool powErrRange(int exValue, int cur);
private:
    bool searchDev();
    bool fsCheck();
    sProgress *mPro;
    sDevData *mDev;
    sObjData *mData;
    Yc_Obj *mYc;
    YC_StandSource *mSource;
    sCfgComIt *mItem;
    Core_Http *http;

private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
