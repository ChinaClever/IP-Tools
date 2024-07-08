/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "logmainwid.h"
#include "ui_logmainwid.h"

LogMainWid::LogMainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogMainWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    QTimer::singleShot(rand()%50,this,SLOT(initFunSLot()));
}

LogMainWid::~LogMainWid()
{
    delete ui;
}


void LogMainWid::initFunSLot()
{
    QString str = tr("打包日志");
    mLogWid = new LogComWid(ui->tabWidget);
    LogBtnBar *bar = new LogBtnBar(); bar->clearHidden();
    mLogWid->initWid(DbLogs::bulid(), bar,  new Log_LogQueryDlg(this), str);
    ui->tabWidget->addTab(mLogWid, str);

    //str = tr("设备Mac记录");
    //mMacWid = new LogComWid(ui->tabWidget);
    //mMacWid->initWid(DbMacs::bulid(),new LogBtnBar(),  new Log_MacQueryDlg(this), str);
    //ui->tabWidget->addTab(mMacWid, str);
}

