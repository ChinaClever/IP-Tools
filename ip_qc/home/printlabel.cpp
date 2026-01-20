#include "printlabel.h"
#include "http/httpclient.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>

printLabel::printLabel(QObject *parent)
    : QObject(parent)
{
}

void printLabel::doPrint(const sLabelData &data)
{
    // 组 CSV
    QString header = "DT,SN,ON,PN,MAC,UUID,QR";

    QString qr = data.qrcode;
    if (qr.isEmpty()) {
        QString base = "https://your-domain.com/label";
        qr = QString("%1?ON=%2&PN=%3&SN=%4")
                 .arg(base)
                 .arg(data.on)
                 .arg(data.pn)
                 .arg(data.sn);
    }

    QString line = QString("%1,%2,%3,%4,%5,%6,%7")
                       .arg(data.dt)
                       .arg(data.sn)
                       .arg(data.on)
                       .arg(data.pn)
                       .arg(data.mac)
                       .arg(data.uuid)
                       .arg(qr);

    QString result = httpPostIni(header + "\n" + line,
                                 "17", "81");

    emit finished(result);
}

QString printLabel::httpPostIni(const QString &data,
                                const QString &ip,
                                const QString &host)
{
    QString url = QString("http://192.168.1.%1:%2/Integration/Mpro/Execute")
    .arg(ip)
        .arg(host);

    AeaQt::HttpClient http;
    QEventLoop loop;
    QString result;

    QObject::connect(&http, &AeaQt::HttpClient::finished,
                     &loop, &QEventLoop::quit);

    http.post(url)
        .header("content-type", "plain")
        .onSuccess([&](QString res) {
            result = "SUCCESS:" + res;
            loop.quit();
        })
        .onFailed([&](QString err) {
            result = "ERROR:" + err;
            loop.quit();
        })
        .onTimeout([&](QNetworkReply *) {
            result = "ERROR:timeout";
            loop.quit();
        })
        .timeout(5000)
        .body(data.toUtf8())
        .exec();

    loop.exec();
    return result;
}
