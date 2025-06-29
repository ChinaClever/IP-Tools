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
    header << tr("电压") << tr("电流") << tr("有功功率")
            << tr("功率因素")  << tr("有功电能");
    initTableWid(header, 3, title);
}

void Home_LineTabWid::appendItem()
{
    sPdudata *it = &Core_Object::coreItem.actual.value;
    if(Core_Object::coreItem.actual.param.loopNum > 0) {
        for(int i=0; i<it->loopVol.size(); ++i) {
            QStringList listStr;
            listStr << QString::number(it->loopVol.at(i).toDouble(),'f',2)+"V";
            listStr << QString::number(it->loopCur.at(i).toDouble(),'f',2)+"A";
            listStr << QString::number(it->loopPow.at(i).toDouble(),'f',3)+"kW";
            listStr << QString::number(it->loopPF.at(i).toDouble(),'f',2);
            listStr << QString::number(it->loopEle.at(i).toDouble(),'f',1)+"kWh";
            setTableRow(i, listStr);
        }
    } else {
        for(int i=0; i<it->lineVol.size(); ++i) {
            QStringList listStr;
            listStr << QString::number(it->lineVol.at(i).toDouble(),'f',2)+"V";
            listStr << QString::number(it->lineCur.at(i).toDouble(),'f',2)+"A";
            listStr << QString::number(it->linePow.at(i).toDouble(),'f',3)+"kW";
            listStr << QString::number(it->linePF.at(i).toDouble(),'f',2);
            listStr << QString::number(it->lineEle.at(i).toDouble(),'f',1)+"kWh";
            setTableRow(i, listStr);
        }
    }
}

void Home_LineTabWid::timeoutDone()
{
    clearTable();
    appendItem();
}
