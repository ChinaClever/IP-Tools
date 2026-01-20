#ifndef JSONPACK_H
#define JSONPACK_H

#include "datapacket.h"
#include "http/httpclient.h"

class Json_Pack:public QObject
{
    Q_OBJECT
    explicit Json_Pack(QObject *parent = nullptr);
public:
    static Json_Pack *bulid(QObject *parent = nullptr);
     void http_post(const QString &method, QJsonObject &json, const int id, const QString &ip, int port=48088);

    sProgress* getPro() const { return mPro; }

    void head(QJsonObject &obj);
signals:
    void httpSig(const QString &msg, bool pass, const QString Request, const QString testStep, const QString testItem);


protected:
    void pduInfo(QJsonObject &json,const int id);
    int objData(QJsonObject &obj,const int id);

private:
    sProgress *mPro;
};

#endif // JSONPACK_H
