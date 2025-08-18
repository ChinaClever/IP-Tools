/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "printer_bartender.h"
#include "print.h"
#include <QtPrintSupport/QPrinterInfo>

Printer_BarTender::Printer_BarTender(QObject *parent) : QObject(parent)
{
    mSocket = new QUdpSocket(this);
    mSocket->bind(QHostAddress::Any, 37755);
    connect(mSocket,SIGNAL(readyRead()),this,SLOT(recvSlot()));
}

Printer_BarTender *Printer_BarTender::bulid(QObject *parent)
{
    static Printer_BarTender* sington = nullptr;
    if(sington == nullptr)
        sington = new Printer_BarTender(parent);
    return sington;
}

QString Printer_BarTender::createOrder(sBarTend &it)
{
    QString str = "PN,HW,FW,Date,SN,QR\n";
    str += it.pn + ","; str += it.hw + ","; str += it.fw + ",";
    QString date = QDate::currentDate().toString("yy") + "W";
    date += QString("%1").arg(QDate::currentDate().weekNumber(), 2, 10, QLatin1Char('0'));
    str += date+","; str += "SN " + it.sn + ","; //it.sn = it.sn.remove(QRegExp("\\s"));
    str += QString("G$K:%1%$S:%2%M:%3$HW:%4$FW%5").arg(it.pn).arg(it.sn).arg(date).arg(it.hw).arg(it.fw);
    return str;
}

void Printer_BarTender::delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

bool Printer_BarTender::recvResponse(int sec)
{
    mRes = false;
    for(int i=0; i<10*sec; ++i) {
        if (mRes) break; else delay(100);
    }

    return mRes;
}

bool Printer_BarTender::printer(sBarTend &it)
{
    int port = 1044;
    QHostAddress addr = QHostAddress::Broadcast;
    QString order = createOrder(it);
    sendMsg(order.toLocal8Bit(), port+1, addr);

    sendMsg("start", port, addr);
    return recvResponse(3);
}

int Printer_BarTender::sendMsg(const QByteArray &msg, quint16 port, const QHostAddress &host)
{
    int ret = mSocket->writeDatagram(msg, host, port);
    if(ret > 0) {mSocket->flush(); delay(100);}
    else qDebug() << mSocket->errorString();
    return ret;
}

void Printer_BarTender::recvSlot()
{
    while (mSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = mSocket->receiveDatagram();
        if(datagram.data().size()) mRes = true;
    }
}

/**
 * @brief 发送原始数据到打印机（ZPL 命令）
 */
bool Printer_BarTender::RawDataToPrinter(LPSTR szPrinterName, LPBYTE lpData, DWORD dwCount)
{
    bool        bStatus = FALSE;
    HANDLE      hPrinter = NULL;
    DOC_INFO_1A DocInfo;
    DWORD       dwJob = 0L;
    DWORD       dwBytesWritten = 0L;

    // 打开打印机
    bStatus = OpenPrinterA(szPrinterName, &hPrinter, NULL);
    if (bStatus)
    {
        // 填写打印文档信息
        DocInfo.pDocName = (LPSTR)"Raw Document";
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = (LPSTR)"RAW";

        // 通知后台处理程序文档正在开始
        dwJob = StartDocPrinterA(hPrinter, 1, (LPBYTE)&DocInfo);
        if (dwJob > 0)
        {
            // 开始一页的打印
            bStatus = StartPagePrinter(hPrinter);
            if (bStatus)
            {
                // 发送数据到打印机
                bStatus = WritePrinter(hPrinter, lpData, dwCount, &dwBytesWritten);
                EndPagePrinter(hPrinter);
            }
            EndDocPrinter(hPrinter);
        }
        ClosePrinter(hPrinter);
    }

    if (!bStatus || (dwBytesWritten != dwCount)) {
        return false;
    }

    return true;
}


/**
 * @brief 打印 MAC 条码标签
 * @param cmd MAC地址（例如：D0:12:34:56:78:9A）
 */
bool Printer_BarTender::printMAC(QString cmd)
{
    QString zpl = tr("^XA^PW2000^LL800^LH0,0^MD13^PR2"
                     "^FO20,5^BY2.5,3,65^BCN,65,N,,,^FD%1^FS"
                     "^FO225,75^A0,40,40^FD%1^FS^XZ")
                     .arg("MAC:" + cmd);

    QString printerName = QPrinterInfo::defaultPrinterName();
    return RawDataToPrinter((LPSTR)printerName.toLocal8Bit().data(),
                            (LPBYTE)zpl.toLocal8Bit().data(), zpl.length());
}


/**
 * @brief 打印 FW 标签
 */

bool Printer_BarTender::printerInfo(sBarTend &it)
{
    QString cmd = "^XA^PW1000^LL800^LH0,0^MD13^PR2";
    cmd += QString("^FO10,50^A0,45,45^FDPN:%1     Date:%2^FS")
               .arg(it.pn)
               .arg(QDate::currentDate().toString("yyyy-MM-dd"));
    cmd += QString("^FO10,120^A0,45,45^FDFW:%1     HW:%2^FS").arg(it.fw).arg(it.hw);
    cmd += "^XZ";

    QString printerName = QPrinterInfo::defaultPrinterName();
    return RawDataToPrinter((LPSTR)printerName.toLocal8Bit().data(),
                            (LPBYTE)cmd.toLocal8Bit().data(), cmd.length());
}
