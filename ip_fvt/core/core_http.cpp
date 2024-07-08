/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_http.h"


Core_Http::Core_Http(QObject *parent)
    : QObject{parent}
{
    initHost();
}

Core_Http *Core_Http::bulid(QObject *parent)
{
    static Core_Http* sington = nullptr;
    if(!sington) sington = new Core_Http(parent);
    return sington;
}

void Core_Http::initHost(const QString &ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void Core_Http::execute(const QString cmd)
{
    QJsonObject json;
    json.insert("cmd", cmd);
    http_put("execute", json, m_ip, m_port);
}

void Core_Http::setting(const sDataItem &it)
{
    QJsonObject json, obj;
    json.insert("addr",  it.addr);
    json.insert("type", it.type);
    json.insert("topic", it.topic);
    json.insert("subtopic", it.subtopic);
    json.insert("id", it.id);
    json.insert("value", it.value);

    obj.insert("pduDataSet", json);
    http_post("pduSetting", obj, m_ip, m_port);
}

void Core_Http::setting(const sCfgItem &it, const QVariant &value)
{
    QJsonObject json, obj;
    json.insert("addr", it.addr);
    json.insert("type", it.type);
    json.insert("fc", it.fc);
    json.insert("id", it.id);
    json.insert("value", value.toJsonValue());

    obj.insert("pduCfgSet", json);
    http_post("pduSetting", obj, m_ip, m_port);
}

void Core_Http::uploadFile(const QStringList &fs)
{
    foreach (const auto &fn, fs) {
        if(QFile::exists(fn)) {
            if(File::fileSize(fn) > 3)
            http_upload("upload", fn, m_ip, m_port);
        } else qDebug() << fn;
        cm_mdelay(350); //qDebug() << fn;
    }
}

bool Core_Http::uploadFile(const QString &fn)
{
    bool ret = QFile::exists(fn);
    if(ret && File::fileSize(fn) > 3)
        http_upload("upload", fn, m_ip, m_port);
    else qDebug() << fn;
    return ret;
}

void Core_Http::downFile(const QStringList &fs)
{
    foreach (const auto &fn, fs) {
        QJsonObject json; json.insert("file", fn);
        bool ret = http_down("download", json, fn, m_ip, m_port);
        if(ret) cm_mdelay(243); else break;
    }
}

auto Core_Http::sslConfig()
{
    QSslConfiguration SSLConfig;
    SSLConfig = QSslConfiguration::defaultConfiguration();
    SSLConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    //SSLConfig.setProtocol(QSsl::TlsV1_2OrLater);
    return SSLConfig;
}




void Core_Http::http_get(const QString &method, QJsonObject &json, const QString &ip, int port)
{
    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "https://%1:%2/%3";
    http.get(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(sslConfig())
            .timeout(1000) // 1s超时
            .body(json)
            .block()
            .exec();
}


void Core_Http::http_post(const QString &method, QJsonObject &json, const QString &ip, int port)
{
    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "https://%1:%2/%3";
    http.post(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(sslConfig())
            .timeout(1000) // 1s超时
            .body(json)
            .block()
            .exec();
}


void Core_Http::http_put(const QString &method, QJsonObject &json, const QString &ip, int port)
{
    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "https://%1:%2/%3";
    http.put(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .onSuccess([&](QString result) {emit httpSig(result);})
    .onFailed([&](QString error) {emit httpSig(error); })
    .onTimeout([&](QNetworkReply *) {emit httpSig("timeout"); }) // 超时处理
    .sslConfiguration(sslConfig())
            .timeout(1000) // 1s超时
            .body(json)
            .block()
            .exec();
}


bool Core_Http::http_down(const QString &method, QJsonObject &json, const QString &file, const QString &ip, int port)
{
    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "https://%1:%2/%3"; bool ret = true;
    http.get(url.arg(ip).arg(port).arg(method))
            .header("content-type", "application/json")
            .sslConfiguration(sslConfig())
            .download(file)  // 启用自动设置文件名字
            .onDownloadFileSuccess([&](QString fileName) { emit httpSig("Download completed: "+fileName);})
    .onDownloadFileFailed([&](QString error) { emit httpSig("Download failed: "+error+": "+file); ret = false;})
    .timeout(2500) // 1s超时
            .body(json)
            .block()
            .exec();
    return ret;
}

void Core_Http::http_upload(const QString &method, const QString &file, const QString &ip, int port)
{
    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "https://%1:%2/%3";
    http.post(url.arg(ip).arg(port).arg(method)).body(file, file)
            .onSuccess([&](QString result) {emit httpSig("Upload completed: "+result);})
    .onFailed([&](QString error) {emit httpSig("Upload failed: "+file+error);})
    .sslConfiguration(sslConfig())
            .timeout(1000) // 1s超时
            .block()
            .exec();
}
