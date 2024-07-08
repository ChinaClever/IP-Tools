/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mNavBarWid = new NavBarWid(ui->barWid);
    QTimer::singleShot(50,this,SLOT(initFunSlot())); //延时初始化
    connect(mNavBarWid, SIGNAL(navBarSig(int)), this, SLOT(navBarSlot(int)));

    // 查詢Qt支持的版本
    bool bSupp = QSslSocket::supportsSsl();
    QString buildVersion = QSslSocket::sslLibraryBuildVersionString();
    QString version = QSslSocket::sslLibraryVersionString();
    qDebug() << bSupp << buildVersion << version;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFunSlot()
{
    mHomeWid = new Home_MainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mHomeWid);

    mStatus = new Pro_StatusWid(ui->stackedWid);
    ui->stackedWid->addWidget(mStatus);
    connect(mHomeWid, SIGNAL(startSig()), mStatus, SLOT(startSlot()));

    mResult = new Pro_ResultWid(ui->stackedWid);
    ui->stackedWid->addWidget(mResult);
    connect(mHomeWid, SIGNAL(startSig()), mResult, SLOT(startSlot()));

    mRecvWid = new Remote_RecvWid(ui->stackedWid);
    ui->stackedWid->addWidget(mRecvWid);
    connect(mHomeWid, SIGNAL(startSig()), mRecvWid, SLOT(startSlot()));

    mSwWid = new Remote_SwVerWid(ui->stackedWid);
    ui->stackedWid->addWidget(mSwWid);

    bool bSupp = QSslSocket::supportsSsl();
    if(!bSupp) MsgBox::critical(this, tr("缺少ssl库，无法进行升级"));
}

void MainWindow::navBarSlot(int id)
{
    ui->stackedWid->setCurrentIndex(id);
}
