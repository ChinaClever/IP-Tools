/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_serialnumber.h"

Core_Object::Core_Object(QObject *parent)
    : QThread{parent}
{
    initCurrentNum();
}

void Core_Object::writeMac(const QByteArray &mac)
{
    QFile file("usr/data/pdu/cfg/mac.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(mac); file.close();
    } wirteCfgMac();
}


void Core_Object::wirteCfgMac()
{
    sMac *it = MacAddr::bulid()->macItem;
    CfgCom *cfg = CfgCom::bulid();
    cfg->writeCfg("mac", it->mac, "Mac");
    cfg->writeCfg("cnt", it->cntMac, "Mac");
    cfg->writeCfg("start", it->startMac, "Mac");
    cfg->writeCfg("end", it->endMac, "Mac");
}


QString Core_Object::updateMacAddr(int step)
{
    sMac *it = MacAddr::bulid()->macItem;
    mPro = sDataPacket::bulid()->getPro();

    if(it->mac.size() > 5) {
#if 1
        MacAddr *mac = MacAddr::bulid();
        it->mac = mac->macAdd(it->mac, step);
        writeMac(it->mac.toLatin1());
#else
        QString url = "mac/test?work_order=%1&serial_id=%2";
        url = url.arg(mPro->pn).arg(mPro->productSN);
        sleep(1);
        QString temp = Json_Pack::bulid()->http_get(url,"192.168.1.12");
        QJsonDocument jsonDoc = QJsonDocument::fromJson(temp.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        it->mac = jsonObj["mac_address"].toString();
#endif
        CfgCom::bulid()->writeCfg("mac", it->mac, "Mac");
    } else {
        qDebug() << "updateMacAddr err" << it->mac;
    }

    return it->mac;
}
QString Core_Object::createSn()
{
    QString cmd = "2I3"; mCurrentNum +=1;
    int m = QDate::currentDate().month();
    int y = QDate::currentDate().year() - 2000;
    for(int i=0; i<3; ++i) cmd += "%" + QString::number(i+1);
    QString sn  = QString(cmd).arg(y,2,10,QLatin1Char('0'))
                     .arg(m, 2, 10,QLatin1Char('0'))
                     .arg(mCurrentNum, 5, 10, QLatin1Char('0'));
    this->setCurrentNum();
    return sn.toUpper();
}


void Core_Object::setDate()
{
    QString value = QDate::currentDate().toString("yyyy-MM-dd");
    CfgCom::bulid()->writeCfg("date", value, "Date");
}

bool Core_Object::getDate()
{
    QString str = CfgCom::bulid()->readCfg("date","","Date").toString();
    bool ret = false; if(!str.isEmpty()) {
        QDate date = QDate::fromString(str, "yyyy-MM-dd");
        QDate cu = QDate::currentDate();
        date.setDate(date.year(), date.month(), cu.day());
        if(cu > date) ret = true;
    }

    return ret;
}

void Core_Object::setCurrentNum()
{
    setDate(); CfgCom *cfg = CfgCom::bulid();
    cfg->writeCfg("num", mCurrentNum, "Date");
}

void Core_Object::initCurrentNum()
{
    bool ret = getDate();
    if(ret) {
        mCurrentNum = 0;
        setCurrentNum();
    } else {
        CfgCom *cfg = CfgCom::bulid();
        int value = cfg->readCfg("num", 1,"Date").toInt();
        mCurrentNum = value;
    }
}
