#ifndef JSONRPCCLIENT_H
#define JSONRPCCLIENT_H

#include "jsonrpcobj.h"

class JsonRpcClient : public JsonRpcObj
{
    Q_OBJECT
public:
    explicit JsonRpcClient(QObject *parent = nullptr);
    double pduDataGet(uchar addr,  uchar type, uchar topic, uchar sub, uchar id);
    bool pduDataSet(uchar addr,  uchar type, uchar topic, uchar sub, uchar id, double value);

    QVariant pduCfgGet(uchar type, uchar fc, uchar id=0, uchar addr=0);
    bool pduCfgSet(uchar type, uchar fc, const QVariant &value, uchar id=0, uchar addr=0);
    bool pduRelaysCtrl(int addr, int start, int num, uchar on);
    QString pduLogFun(uchar type, uchar fc, int id, int cnt=30);
    QString pduMetaData(uchar addr);
    QString execute(const QString &cmd);
signals:
    void rpcSig(const QVariant &msg);
};

#endif // JSONRPCCLIENT_H
