/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "jsonrpcclient.h"

JsonRpcClient::JsonRpcClient(QObject *parent)
    : JsonRpcObj{parent}
{

}

QString JsonRpcClient::execute(const QString &cmd)
{
    QString ret;
    auto result = rpc_client->call("execute", cmd);
    if (result->isSuccess()) {
        ret = result->result().toString();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

QString JsonRpcClient::pduMetaData(uchar addr)
{
    QString ret;
    auto result = rpc_client->call("pduMetaData", addr);
    if (result->isSuccess()) {
        ret = result->result().toString();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

double JsonRpcClient::pduDataGet(uchar addr,  uchar type, uchar topic, uchar sub, uchar id)
{
    double ret = -1;
    auto result = rpc_client->call("pduDataGet", addr, type, topic, sub, id);
    if (result->isSuccess()) {
        ret = result->result().toDouble();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

bool JsonRpcClient::pduDataSet(uchar addr,  uchar type, uchar topic, uchar sub, uchar id, double value)
{
    bool ret = false;
    auto result = rpc_client->call("pduDataSet", addr, type, topic,sub, id, value);
    if (result->isSuccess()) {
        ret = result->result().toBool();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

QVariant JsonRpcClient::pduCfgGet(uchar type, uchar fc, uchar id, uchar addr)
{
    QVariant ret;
    auto result = rpc_client->call("pduCfgGet", type, fc, id, addr);
    if (result->isSuccess()) {
        ret = result->result();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

bool JsonRpcClient::pduCfgSet(uchar type, uchar fc, const QVariant &value, uchar id, uchar addr)
{
    bool ret = false;
    auto result = rpc_client->call("pduCfgSet", type, fc, value, id, addr);
    if (result->isSuccess()) {
        ret = result->result().toBool();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

bool JsonRpcClient::pduRelaysCtrl(int addr, int start, int num, uchar on)
{
    bool ret = false;
    auto result = rpc_client->call("pduRelaysCtrl", addr, start, num, on);
    if (result->isSuccess()) {
        ret = result->result().toBool();
        emit rpcSig(ret);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }

    return ret;
}

QString JsonRpcClient::pduLogFun(uchar type, uchar fc, int id, int cnt)
{
    QString str;
    auto result = rpc_client->call("pduLogFun", type, fc, id, cnt);
    if (result->isSuccess()) {
        str = result->result().toString(); emit rpcSig(str);
    } else {
        emit rpcSig(result->toString());
        qDebug() << Q_FUNC_INFO << "RPC error:" << result->toString();
    }
    return str;
}
