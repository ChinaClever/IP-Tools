/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "remote_recvwid.h"
#include "ui_remote_recvwid.h"
#include "backcolourcom.h"
#include "core_sender.h"
#include <QDateTime>

Remote_RecvWid::Remote_RecvWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Remote_RecvWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 21437, QUdpSocket::ShareAddress);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Remote_RecvWid::udpRecvSlot);
}

Remote_RecvWid::~Remote_RecvWid()
{
    delete ui;
}

void Remote_RecvWid::startSlot()
{
    ui->textEdit->clear();  mCnt = 0; mErrIp.clear();
}

void Remote_RecvWid::setTextColor(const QString &str)
{
    bool pass = true; if(str.size() < 120) {
    if(str.contains("err") || str.contains("Err")) pass = false;
    if(str.contains("fail") || str.contains("Fail")) pass = false;
    if(str.contains("错误") || str.contains("失败")) pass = false; }

    QColor color("black"); if(!pass) color = QColor("red");
    if(str.contains("成功") || str.contains("now reboot")) color = QColor("green");
    ui->textEdit->moveCursor(QTextCursor::Start);

    QTextCharFormat fmt;//文本字符格式
    fmt.setForeground(color);// 前景色(即字体色)设为color色
    QTextCursor cursor = ui->textEdit->textCursor();//获取文本光标
    cursor.mergeCharFormat(fmt);//光标后的文字就用该格式显示
    ui->textEdit->mergeCurrentCharFormat(fmt);//textEdit使用当前的字符格式
}

void Remote_RecvWid::upgradeResults(const QString &dst, const QString &str)
{
    if(str.size() < 120 && (str.contains("err") || str.contains("Err"))) {
        if(str.contains("continue")) return;
        if(mErrIp.contains(dst)) return; else mErrIp << dst;
        emit Core_Sender::bulid()->upgradeError(dst, str);
    }
}

void Remote_RecvWid::insertText(const QString &dst, const QString &str)
{
    setTextColor(str); upgradeResults(dst, str);
    QString t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz\t");
    if(str.size()>2) ui->textEdit->insertPlainText(t + dst+"\t: "+str+"\n");
    //ui->textEdit->moveCursor(QTextCursor::Start);
//    QTextCursor cursor = ui->textEdit->textCursor();
//    cursor.movePosition(QTextCursor::End);
//    ui->textEdit->setTextCursor(cursor);
    if(mCnt++ > 5000) startSlot();
}

void Remote_RecvWid::udpRecvSlot()
{
    QByteArray datagram; QHostAddress host;
    while(udpSocket->hasPendingDatagrams()) {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        int ret = udpSocket->readDatagram(datagram.data(), datagram.size(), &host);
        if(ret > 0) insertText(host.toString().remove("::ffff:"), QString(datagram).remove("::ffff:"));
        else qCritical() << udpSocket->errorString();
    }
}
