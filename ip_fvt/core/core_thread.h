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
    ~Core_Thread();

    void setIps(const QStringList &ips) {m_ips = ips;}
    QString m_mac, m_sn;
    QStringList getFs();
    void run();
    bool initFun();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);
    void overSig(); void startSig();
    void msgSigYC(const QString &msg, bool pass);

    void requestDownVer(const QString &ip);

public slots:
    void startSlot() {run();}
    void udpRecvSlot();

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
    bool volErrRangeByID(int i, bool flag);
    bool volErrRange();
    bool curErrRange(int exValue, int cur);
    bool powErrRange(int exValue, int cur);

    bool noLoadCurFun();
    bool noLoadCurCheck(int cnt);
    bool waitForRest();

private:
    bool searchDev();
    bool fsCheck();
    sObjData *mData;
    Yc_Obj *mYc;
    YC_StandSource *mSource;
    sCfgComIt *mItem;
    Core_Http *http;
    QUdpSocket *udpSocket;

private:
    QStringList m_ips;
    QString data;
};

#endif // CORE_THREAD_H
