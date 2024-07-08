#include "pro_statuswid.h"
#include "ui_pro_statuswid.h"

Pro_StatusWid::Pro_StatusWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pro_StatusWid)
{
    ui->setupUi(this);
    groupBox_background_icon(this);
    QTimer::singleShot(150,this,SLOT(initFunSlot()));
}

Pro_StatusWid::~Pro_StatusWid()
{
    delete ui;
}

void Pro_StatusWid::initFunSlot()
{
    mTable = new Pro_TableWid(ui->groupBox);
    Core_Sender *sender = Core_Sender::bulid(this);
    connect(sender, &Core_Sender::devListSig, this, &Pro_StatusWid::updateIps);
    connect(sender, &Core_Sender::subProgress, this, &Pro_StatusWid::subProgressSlot);

    QStringList header;
    header << tr("目标设备") << tr("传输进度");
    mTable->initTableWid(header, 0);
}

void Pro_StatusWid::startSlot()
{
    mMap.clear();
    mTable->delTable();
}

void Pro_StatusWid::updateIps(const QStringList& ips)
{
    startSlot();
    for(int i=0; i<ips.size(); ++i) {
        mMap.insert(ips.at(i), i);
        mTable->setTableItem(i, 0, ips.at(i));
    }
}

void Pro_StatusWid::subProgressSlot(const QString &ip ,int v)
{
    int id = mMap.value(ip);
    if(id >= 0) {
        mTable->setTableItem(id, 1, QString::number(v));
    }
}
