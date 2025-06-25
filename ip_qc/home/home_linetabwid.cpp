/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_linetabwid.h"

Home_LineTabWid::Home_LineTabWid(QWidget *parent) : ComTableWid(parent)
{
    initWid();
}

void Home_LineTabWid::initWid()
{
    QString title = tr("相数据列表"); QStringList header;
    header  << tr("电流") << tr("电压") << tr("有功功率");
    initTableWid(header, 3, title);
}

void Home_LineTabWid::appendItem()
{
    sPdudata *it = &Core_Object::coreItem.actual.value;
    for(int i=0; i<it->lineVol.size(); ++i) {
        QStringList listStr;

        //if(1 == dev->sw[i]) listStr << tr("开"); else listStr << tr("关");
        listStr << QString::number(it->lineCur.at(i).toDouble(),'f',2)+"A";
        listStr << QString::number(it->lineVol.at(i).toDouble(),'f',2)+"V";
        listStr << QString::number(it->linePow.at(i).toDouble(),'f',3)+"kW";
        setTableRow(i, listStr);

        // setItemColor(i, 1, dev->cur.status[i]);
        // setItemColor(i, 2, dev->vol.status[i]);
        // setItemColor(i, 3, dev->powStatus[i]);
    }
}

void Home_LineTabWid::timeoutDone()
{
    clearTable();
    appendItem();
}
