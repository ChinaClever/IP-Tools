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
    header << tr("电压") << tr("电流") << tr("有功功率"); //<< tr("功率因素")  << tr("有功电能");
    initTableWid(header, 3, title);
}

void Home_LineTabWid::appendItem()
{
    sThreshold *it = &Core_Object::coreItem.actual.value;
    int num = Core_Object::coreItem.actual.param.lineNum;
    for(int i=0; i<num; ++i) {
        QStringList listStr;        
        listStr << QString::number(it->lineVol[i],'f',2)+"V";
        listStr << QString::number(it->lineCur[i],'f',2)+"A";
        listStr << QString::number(it->linePow[i],'f',3)+"kW";
        //listStr << QString::number(it->pf[i],'f',2);
        //listStr << QString::number(it->ele[i],'f',1)+"kWh";
        if(it->lineVol[i]) setTableRow(i, listStr);
    }
}

void Home_LineTabWid::timeoutDone()
{
    clearTable();
    appendItem();
}
