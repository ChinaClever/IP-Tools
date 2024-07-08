/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_mainwid.h"
#include "ui_home_mainwid.h"

Home_MainWid::Home_MainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_MainWid)
{
    ui->setupUi(this); initWid();
    groupBox_background_icon(this);
    mWorkWid = new Home_WorkWid(ui->workWid);
    connect(mWorkWid, &Home_WorkWid::startSig, this, &Home_MainWid::onStart);
    connect(mWorkWid, &Home_WorkWid::downSig, this, &Home_MainWid::onDown);
    connect(mWorkWid, &Home_WorkWid::msgSig, this, &Home_MainWid::onMsg);
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    QPalette pl = ui->textEdit->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit->setPalette(pl);
}

void Home_MainWid::onStart()
{
    ui->textEdit->clear();
}

void Home_MainWid::onDown(const QString &dir)
{
    QFile file(dir +"/" + CFG_APP);cm_mdelay(5);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textEdit->setPlainText(file.readAll());
    } file.close();
}

void Home_MainWid::onMsg(const QString &msg)
{
    QString str = tr("输出信息:");
    ui->textEdit->appendPlainText(str);
    ui->textEdit->appendPlainText(msg);
}
