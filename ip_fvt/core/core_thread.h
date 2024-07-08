#ifndef CORE_THREAD_H
#define CORE_THREAD_H

#include "core_serialnumber.h"
#include "json_pack.h"

class Core_Thread : public Core_Object
{
    Q_OBJECT
public:
    explicit Core_Thread(QObject *parent = nullptr);
    void setIps(const QStringList &ips) {m_ips = ips;}
    QString m_mac, m_sn;
    QStringList getFs();
    void run();

signals:
    void msgSig(const QString &msg, bool pass);
    void finshSig(bool pass, const QString &msg);
    void overSig(); void startSig();

public slots:
    void startSlot() {run();}

protected:
    void timeSync();
    void enModbusRtu();
    bool downVer(const QString &ip);
    bool workDown(const QString &ip);
    void writeSnMac(const QString &sn, const QString &mac);

private:
    bool searchDev();
    bool fsCheck();
    sProgress *mPro;

private:
    QStringList m_ips;
};

#endif // CORE_THREAD_H
