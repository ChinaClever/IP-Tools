#ifndef CORE_HTTP_H
#define CORE_HTTP_H
#include "ssdp_core.h"
#include "http/httpclient.h"
#include <QFileDialog>
#include "filemgr.h"
#include "macaddr.h"
#include "cfg_app.h"

class Core_Http : public QObject
{
    Q_OBJECT
    explicit Core_Http(QObject *parent = nullptr);
public:
    static Core_Http *bulid(QObject *parent = nullptr);
    void initHost(const QString &ip="192.168.1.163", int port=3166);
    void downFile(const QStringList &fs);
    void uploadFile(const QStringList &fs);
    bool uploadFile(const QString &fn);
    void setting(const sDataItem &it);
    void setting(const sCfgItem &it, const QVariant &value);
    void execute(const QString cmd);

signals:
    void httpSig(const QString &msg);

private:
    auto sslConfig();
    void http_get(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_post(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_put(const QString &method, QJsonObject &json, const QString &ip, int port);
    void http_upload(const QString &method, const QString &file, const QString &ip, int port);
    bool http_down(const QString &method, QJsonObject &json, const QString &file, const QString &ip, int port);

private:
    //AeaQt::HttpClient mHttp;
    QString m_ip; int m_port;
};

#endif // CORE_HTTP_H
