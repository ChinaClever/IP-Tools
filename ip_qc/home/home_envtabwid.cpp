/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_envtabwid.h"

Home_EnvTabWid::Home_EnvTabWid(QWidget *parent) : ComTableWid(parent)
{
    initWid();
}

void Home_EnvTabWid::initWid()
{
    QString title = tr("传感器信息");
    QStringList header;
    header << tr("传感器") << tr("温度值");
    initTableWid(header, 1, title);
}

void Home_EnvTabWid::appendItem()
{
    sMonitorData *it = &Core_Object::coreItem.actual.data;
    for(int i=0;i<1 && i<it->temps.size(); ++i) {
        QStringList listStr; listStr << QString::number(i+1);
        listStr << QString::number(it->temps.at(i).toDouble(),'f',1)+"℃";
        setTableRow(i, listStr);
    }
}

void Home_EnvTabWid::timeoutDone()
{
    clearTable();
    appendItem();
}
