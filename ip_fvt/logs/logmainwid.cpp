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
    QString str = tr("调试日志");
    mLogWid = new LogComWid(ui->tabWidget);
    LogBtnBar *bar = new LogBtnBar(); //bar->clearHidden();
    mLogWid->initWid(DbLogs::bulid(), bar,  new Log_LogQueryDlg(this), str);
    ui->tabWidget->addTab(mLogWid, str);

    str = tr("Mac记录");
    mMacWid = new LogComWid(ui->tabWidget);
    bar = new LogBtnBar(); bar->clearHidden();
    mMacWid->initWid(DbMacs::bulid(), bar,  new Log_MacQueryDlg(this), str);
    ui->tabWidget->addTab(mMacWid, str);
}

