#ifndef BASELOGS_H
#define BASELOGS_H

#include "dbvalue.h"
#include "baseobject.h"

class BaseLogs : public QThread, public BaseObject
{
    Q_OBJECT
    explicit BaseLogs(QObject *parent = nullptr);
public:
    static BaseLogs *bulid(QObject *parent = nullptr);

    void saveLogs();
    void writeMac(const QString &mac){mMac=mac;}
    bool appendLogItem(const QString &str, bool pass);
    bool appendValueItem(const QString &str, bool pass);

protected:
    void run(){saveLogs();}
    bool initItem(sStateItem &item);
    bool initValueItem(sValueItem &it);
    bool writeLog();
    void writeLogs();
    bool writeMac();
    void writeValues();

private:
    QString mMac;
    QList<sStateItem> mLogItems;
    sValueItem mValueItems;
};

#endif // BASELOGS_H
