#ifndef JSONPACK_H
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
    void http_post(const QString &method, QJsonObject &json, const int id ,const QString &ip,  int port=48088);
    QString http_get(const QString &method, const QString &ip, QJsonObject &obj,int port = 48088);
    void head(QJsonObject &obj);
    const sProgress& getPro() const {
        // 注意：调用者必须确保 Json_Pack 对象存在时才使用该引用
        return *mPro;
    }
signals:
    void httpSig(const QString &msg, bool pass);

protected:
    void part_head(QJsonObject &obj);
    void pduInfo(QJsonObject &json,const int id);
    int objData(QJsonObject &Obj,const int id);

private:
    sProgress *mPro;
};

#endif // JSONPACK_H
