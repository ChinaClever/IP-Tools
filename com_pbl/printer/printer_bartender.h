#ifndef PRINTER_BARTENDER_H
#define PRINTER_BARTENDER_H

#include <QObject>
#include <QtNetwork>

struct sBarTend {
    QString pn;
    QString hw;
    QString fw;
    QString sn;
};

class Printer_BarTender : public QObject
{
    Q_OBJECT
    explicit Printer_BarTender(QObject *parent = nullptr);
public:
    static Printer_BarTender *bulid(QObject *parent = nullptr);
    bool printer(sBarTend &it);

protected:
    void delay(int msec);
    bool recvResponse(int sec);
    QString createOrder(sBarTend &it);
    int sendMsg(const QByteArray &msg, quint16 port,
                const QHostAddress &host=QHostAddress::LocalHost);

protected slots:
    void recvSlot();

private:
    bool mRes;
    QUdpSocket *mSocket;
};

#endif // PRINTER_BARTENDER_H
