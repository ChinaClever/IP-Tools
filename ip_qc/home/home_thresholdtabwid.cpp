/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "home_thresholdtabwid.h"

Home_ThresholdTabWid::Home_ThresholdTabWid(QWidget *parent) : ComTableWid(parent)
{
    initWid();
}

void Home_ThresholdTabWid::initWid()
{
    QString title = tr("报警阈值信息"); QStringList header;
    header << tr("电压额定值") << tr("电流额定值") << tr("功率额定值");
    initTableWid(header, 3, title);
}

void Home_ThresholdTabWid::appendItem()
{    
    sThreshold *it = &Core_Object::coreItem.actual.rate;
    for(int i=0; i<it->lineVol.size(); ++i) {
        QStringList listStr;
        listStr << QString::number(it->lineCur.at(i).toDouble(),'f',2)+"A";
        listStr << QString::number(it->lineVol.at(i).toDouble(),'f',2)+"V";
        listStr << QString::number(it->linePow.at(i).toDouble(),'f',3)+"kW";
        setTableRow(i, listStr);
    }
}

void Home_ThresholdTabWid::timeoutDone()
{
    clearTable();
    appendItem();
}
