#ifndef PRINTLABEL_H
#define PRINTLABEL_H
#include <QString>
#include <QObject>
// 在文件开头或合适位置添加
struct sLabelData
{
    QString dt;      // DT时间 (格式: YYYYMMDD HH:mm)
    QString sn;      // SN序列号
    QString on;      // ON订单号
    QString pn;      // PN成品代码
    QString mac;
    QString uuid;
    QString cn;
    QString qrcode; // 二维码链接
};
Q_DECLARE_METATYPE(sLabelData)
class printLabel : public QObject
{
    Q_OBJECT
public:
    explicit printLabel(QObject *parent = nullptr);

public slots:
    void doPrint(const sLabelData &data);

signals:
    void finished(const QString &result);

private:
    QString httpPostIni(const QString &data,
                        const QString &ip,
                        const QString &host);
};

#endif // PRINTLABEL_H
