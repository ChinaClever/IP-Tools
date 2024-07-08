/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "remote_swverwid.h"
#include "ui_remote_swverwid.h"

Remote_SwVerWid::Remote_SwVerWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Remote_SwVerWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this); mSsdp = Ssdp_Core::bulid(this);
    connect(mSsdp, &Ssdp_Obj::recvSig, this, &Remote_SwVerWid::recvSlot);
}

Remote_SwVerWid::~Remote_SwVerWid()
{
    delete ui;
}

void Remote_SwVerWid::recvSlot(const sSdpIt &it)
{
    if(it.fc == 11) {
        QString data = it.data;
        QString fmd = "room:%1; ip:%2; %3";
        QString str = QString::number(++mId) +"、";
        str += fmd.arg(it.room, it.ip, it.data);
        ui->textEdit->appendPlainText(str);
        QString ct = ui->lineEdit->text();
        if(ct.size() && str.contains(ct)) {
            ui->okTextEdit->appendPlainText(str);
        } else if(ct.size()) {
            ui->errTextEdit->appendPlainText(str);
        }
    }
}

void Remote_SwVerWid::on_startBtn_clicked()
{
    ui->textEdit->clear();
    ui->errTextEdit->clear();
    mId = 0; mSsdp->swVersion();
}

