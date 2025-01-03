﻿#ifndef JSONPACK_H
#define JSONPACK_H

#include "datapacket.h"
#include "http/httpclient.h"
// #include "core_serialnumber.h"

class Json_Pack:public QObject
{
    Q_OBJECT
    explicit Json_Pack(QObject *parent = nullptr);
public:
    static Json_Pack *bulid(QObject *parent = nullptr);
    void http_post(const QString &method, const QString &ip, int port=8000);
    QString http_get(const QString &method, const QString &ip, int port = 8000);
signals:
    void httpSig(const QString &msg, bool pass);

protected:
    void head(QJsonObject &obj);
    void part_head(QJsonObject &obj);
    void pduInfo(QJsonObject &json);
    int objData(QJsonObject &obj);

private:
    sProgress *mPro;
};

#endif // JSONPACK_H
