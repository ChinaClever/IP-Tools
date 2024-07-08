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
    ui->setupUi(this);
    groupBox_background_icon(this);
    initWid();
}

Home_MainWid::~Home_MainWid()
{
    delete ui;
}


void Home_MainWid::initWid()
{
    mTabWid = new Home_StateWid(ui->tabWid);
    mWorkWid = new Home_WorkWid(ui->workWid);
    connect(mWorkWid, SIGNAL(startSig()), mTabWid, SLOT(startSlot()));
    connect(mWorkWid, SIGNAL(startSig()), this, SIGNAL(startSig()));
}
