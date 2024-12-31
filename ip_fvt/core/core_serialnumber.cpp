/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_serialnumber.h"

sCoreItem Core_Object::coreItem;
Core_Object::Core_Object(QObject *parent)
    : QThread{parent}
{
    initCurrentNum();
}

void Core_Object::writeMac(const QByteArray &mac)
{
    QFile file("appconfigs/pdu/cfg/mac.conf");
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
    int pn=CfgCom::bulid()->item->pcNum;
    QString cmd = "3I5"; mCurrentNum +=1;
    int m = QDate::currentDate().month();
    int y = QDate::currentDate().year() - 2000;
    for(int i=0; i<4; ++i) cmd += "%" + QString::number(i+1);
    QString sn  = QString(cmd).arg(y,2,10,QLatin1Char('0'))
                     .arg(m, 2, 10,QLatin1Char('0')).arg(pn)
                     .arg(mCurrentNum, 4, 10, QLatin1Char('0'));
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

bool Core_Object::checkInput(const QByteArray &msg, QJsonObject &obj)
{
    QJsonParseError jsonerror; bool ret = false;
    QJsonDocument doc = QJsonDocument::fromJson(msg, &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError) {
        if(doc.isObject())  {obj = doc.object(); ret = true;}
    }
    return ret;
}

double Core_Object::getData(const QJsonObject &object, const QString &key)
{
    double ret = -1;
    QJsonValue value = getValue(object, key);
    if(value.isDouble()) {
        ret = value.toDouble();
    } else cout << key << object.keys();

    return ret;
}

QJsonValue Core_Object::getValue(const QJsonObject &object, const QString &key)
{
    QJsonValue value;
    if (object.contains(key))  {
        value = object.value(key);
    } else cout << key << object.keys();
    return value;
}

QJsonObject Core_Object::getObject(const QJsonObject &object, const QString &key)
{
    QJsonObject obj;
    if (object.contains(key)){
        QJsonValue value = object.value(key);
        if (value.isObject()){
            obj = value.toObject();
        }
    } else cout << key << object.keys();
    return obj;
}

QJsonArray Core_Object::getArray(const QJsonObject &object, const QString &key)
{
    QJsonArray array;
    if (object.contains(key)) {
        QJsonValue value = object.value(key);
        if (value.isArray()) {
            array = value.toArray();
        } else cout << key << object.keys();
    } else cout << key << object.keys();

    return array;
}

bool Core_Object::jsonAnalysis()
{
    QJsonObject obj; sCoreItem *it = &coreItem;
    QByteArray msg = it->jsonPacket.toLatin1();
    bool ret = checkInput(msg, obj);
    if(ret) {
        // getSn(obj); getMac(obj);
        getParameter(obj);
        it->datetime = getValue(obj, "datetime").toString();
        obj = getObject(obj, "pdu_data");
        getPduData(obj);
    }
    return ret;
}

void Core_Object::getSn(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "pdu_version");
    coreItem.sn = getValue(obj, "serialNumber").toString();
}

void Core_Object::getMac(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "net_addr");
    coreItem.mac = getValue(obj, "mac").toString();
}

void Core_Object::getAlarmStatus(const QJsonObject &object)
{
    coreItem.alarm = getValue(object, "status").toInt();
}

void Core_Object::getParameter(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    QJsonObject obj = getObject(object, "pdu_info");
    it->sensorBoxEn = getData(obj, "sensor_box");
    it->standNeutral = getData(obj, "stand_neutral");
    it->lineNum = getData(obj, "line_num");
    it->loopNum = getData(obj, "loop_num");
    it->language = getData(obj, "language");
    it->isBreaker = getData(obj, "breaker");
    it->vh = getData(obj, "vh");
}

void Core_Object::getTgData(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.actual.data;
    QJsonObject obj = getObject(object, "pdu_tg_data");
    it->apparent_pow = getData(obj, "apparent_pow");
    it->tg_ele = getData(obj, "ele");
    it->tg_pow = getData(obj, "pow");
}

void Core_Object::getEnvData(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.actual.data;
    QJsonObject obj = getObject(object, "env_item_list");
    it->doors = getArray(obj, "door").toVariantList();
    it->temps = getArray(obj, "tem_value").toVariantList();
}

void Core_Object::getPduData(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    sThreshold *item = &coreItem.actual.value;
    int line = (int)it->lineNum;
    int loop = (int)it->loopNum;

    if(loop)
    {
        QJsonObject obj = getObject(object, "loop_item_list");
        for(int i=0;i<loop;i++)
        {
            item->loopVol[i] = getActualValue(obj, "vol", i);
            item->loopCur[i] = getActualValue(obj, "cur", i);
            item->loopPow[i] = getActualValue(obj, "pow", i);
        }
    }else{
        QJsonObject obj = getObject(object, "line_item_list");
        for(int i=0;i<line;i++)
        {
            item->lineVol[i] = getActualValue(obj, "vol", i);
            item->lineCur[i] = getActualValue(obj, "cur", i);
            item->linePow[i] = getActualValue(obj, "pow", i);
        }
    }
}

double Core_Object::getActualValue(const QJsonObject &object, const QString &key, int value, const QString &suffix)
{
    QJsonArray array = getArray(object, key+"_"+suffix);
    if(suffix.contains("value")) {
        for (int i = 0; i < array.size(); ++i) {
            if (i == value) {
                break;
            }
        }
    }

    return array.at(value).toDouble();
}

double Core_Object::getRating(const QJsonObject &object, const QString &key, const QString &suffix)
{
    QJsonArray array = getArray(object, key+"_"+suffix);
    QJsonValue firstElement = array.first();
    bool allElementsEqual = true;
    if(suffix.contains("rated")) {
        for (int i = 1; i < array.size(); ++i) {
            if (firstElement != array.at(i)) {
                allElementsEqual = false;
                break;
            }
        }
    }

    if(!allElementsEqual) cout <<  array;
    return array.first().toDouble();
}



