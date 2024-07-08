/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "printer_bartender.h"
#include "print.h"

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
