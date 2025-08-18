#ifndef PRINTER_BARTENDER_H
#define PRINTER_BARTENDER_H

#include <QObject>
#include <QtNetwork>
#include <windows.h>

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
    // 添加以下两个函数声明（靠近 public 区域即可）
    bool printMAC(QString cmd);          // 打印 MAC 条码
    bool printerInfo(sBarTend &it);      // 打印标签（PN, HW, FW）

protected:
    void delay(int msec);
    bool recvResponse(int sec);
    QString createOrder(sBarTend &it);
    int sendMsg(const QByteArray &msg, quint16 port,
                const QHostAddress &host=QHostAddress::LocalHost);


    bool RawDataToPrinter(LPSTR szPrinterName, LPBYTE lpData, DWORD dwCount);

protected slots:
    void recvSlot();

private:
    bool mRes;
    QUdpSocket *mSocket;
};

#endif // PRINTER_BARTENDER_H
