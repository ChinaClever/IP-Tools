/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "navbarwid.h"
#include "ui_navbarwid.h"
#include "backcolourcom.h"

NavBarWid::NavBarWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavBarWid)
{
    ui->setupUi(this);
    set_background_icon(this,":/image/title_back.jpg");
    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->setContentsMargins(0, 0, 0, 6);
    gridLayout->addWidget(this);
}

NavBarWid::~NavBarWid()
{
    delete ui;
}


