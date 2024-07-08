#ifndef AD_MACADDR_H
#define AD_MACADDR_H
#include <QtCore>

struct sMac
{
    sMac() {cntMac=0;}
    int cntMac;
    QString mac;
    QString user;
    QString startMac;
    QString endMac;
};

class MacAddr
{
    MacAddr();
public:
    static MacAddr *bulid();
    sMac *macItem;
    bool isMacAddress(const QString &mac);

    QString macAdd(const QString &in, int step);
    int macToChar(const QString &in, uchar *buf);
    int macCnt(const QString &start, const QString &end);

protected:
    QString intToMac(quint64 v);
    int intToChar(quint64 v, uchar *buf);
    quint64 macToInt(const QString &in);
    QByteArray toLittleEndian(quint64 v);
    quint64 fromBigEndian(const QByteArray &bytes);
    QByteArray get_mac_array_from_QString(const QString &in);
};

#endif // AD_MACADDR_H
