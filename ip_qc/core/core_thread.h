#ifndef CORE_THREAD_H
#define CORE_THREAD_H
#include "json_pack.h"
#include "core_object.h"

class Core_Thread : public Core_Object
{
    Q_OBJECT
    explicit Core_Thread(QObject *parent = nullptr);
public:
    static Core_Thread *bulid(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    void run();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);    
    void overSig();

private:
    bool readDev(const QString &ip="192.168.1.17");
    bool fwCheck();
    bool snCheck();
    bool envCheck();
    bool linkCheck();
    bool macCheck();
    bool searchDev();
    bool timeCheck();
    bool alarmCheck();
    bool devNumCheck();
    bool parameterCheck();
    bool loopthresholdCheck(int i);
    bool linethresholdCheck(int i);
    bool compareImages();
    bool bigEleCheck();
    bool logoCheck(const QString &ip);
    bool downLogo(const QString &ip);
    bool workDown(const QString &ip);
    bool cpuCheck();
    bool errRangeCheck();

    bool volErrRange(int i, bool flag);
    bool curErrRange(int i, bool flag);
    bool powErrRange(int i, bool flag);
    bool eleErrRange();
    bool apowErrRange();
    bool apeleErrRange();

    QString changeType(int value);
    QString changeSpec(int value);
    QString changeProtocol(int value);
    bool checkAlarmErr();
    bool checkErrRange(int exValue, int value, int err);
    bool checkSquare(int exValue, int value, int err);

    void initReadCmd(sRtuItem &item);
private:
    QString mLogo;
    QStringList m_ips;
    QHash<QString, QString> mHashMac, mHashSn;
    sCfgComIt *mItem;
    Core_Http *http;
};

#endif // CORE_THREAD_H
