/*
 *
 *  Created on: 2022年1月1日
 *      Author: Lzy
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mNavBarWid = new NavBarWid(ui->barWid);
    QTimer::singleShot(50,this,SLOT(initFunSlot())); //延时初始化
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initFunSlot()
{
    mHomeWid = new Home_MainWid(ui->stackedWid);
    ui->stackedWid->addWidget(mHomeWid);
}


