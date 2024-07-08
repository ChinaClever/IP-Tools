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
    bool fwCheck();
    bool tgCheck();
    bool snCheck();
    bool envCheck();
    bool macCheck();
    bool searchDev();
    bool timeCheck();
    bool alarmCheck();
    bool devNumCheck();
    bool outletCheck();
    bool mcuTempCheck();
    bool outputVolCheck();
    bool supplyVolCheck();
    bool parameterCheck();
    bool thresholdCheck();
    bool compareImages();
    bool bigEleCheck();
    bool logoCheck(const QString &ip);
    bool downLogo(const QString &ip);
    bool workDown(const QString &ip);

private:
    QString mLogo;
    QStringList m_ips;
};

#endif // CORE_THREAD_H
