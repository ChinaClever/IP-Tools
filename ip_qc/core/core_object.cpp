/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_object.h"

sCoreItem Core_Object::coreItem;
Core_Object::Core_Object(QObject *parent)
    : QThread{parent}
{
    mHttp = Core_Http::bulid(this);
}

void Core_Object::clearAllEle()
{
    sDataItem it;
    it.id = 0;
    it.type = 3;
    it.topic = 5;
    it.subtopic = 1;
    mHttp->setting(it);
}

void Core_Object::enCascade(int v)
{
    sCfgItem it;
    it.type = 13; it.fc = 3;
    mHttp->setting(it, v);
}

void Core_Object::rtuSet(int v)
{
    sCfgItem it;
    it.type = 15; it.fc = 1;
    mHttp->setting(it, v);
    it.fc = 7; mHttp->setting(it, 1);
}

void Core_Object::boxSet(int v)
{
    sCfgItem it;
    it.type = 13; it.fc = 9;
    mHttp->setting(it, v);
}

void Core_Object::factoryRestore()
{
    sCfgItem it;
    it.type = 96; it.fc = 2;
    mHttp->setting(it, 1);
}

void Core_Object::setRunTime()
{
    sCfgItem it;
    it.type = 13; it.fc = 5;
    mHttp->setting(it, 0);
}

void Core_Object::clearLogs()
{
    sCfgItem it;
    it.type = 96; it.fc = 3;
    mHttp->setting(it, 0);
}

void Core_Object::relayCtrl(int on, int id)
{
    sDataItem it;
    it.id = id;
    it.type = 3;
    it.topic = 1;
    it.subtopic = 1;
    it.value = on;
    mHttp->setting(it);
}

void Core_Object::relayDelay(int sec, int id)
{
    sDataItem it;
    it.id = id;
    it.type = 3;
    it.topic = 1;
    it.subtopic = 4;
    it.value = sec;
    mHttp->setting(it);
}

void Core_Object::readMetaData()
{
    mHttp->readJson(0);
    //cm_mdelay(455);
}

void Core_Object::timeSync()
{
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QString t = QDateTime::currentDateTime().toString(fmd);
    //emit msgSig(tr("时间设置:")+t, true);
    Core_Http *http = Core_Http::bulid(this);
    sCfgItem it; it.type = 43; it.fc =1;
    http->setting(it, t);
}

void Core_Object::irqCheck()
{
    QString cmd = "cat /tmp/kernel_messages";
    Core_Http::bulid()->execute(cmd);
}


bool Core_Object::jsonAnalysis()
{
    QJsonObject obj; sCoreItem *it = &coreItem;
    QByteArray msg = it->jsonPacket.toLatin1();
    bool ret = checkInput(msg, obj);
    if(ret) {
        getOutputVol(obj); getSn(obj);
        getMac(obj); getParameter(obj); getAlarmStatus(obj);
        it->datetime = getValue(obj, "datetime").toString();
        obj = getObject(obj, "pdu_data"); getThreshold(obj);
        getTgData(obj); getEnvData(obj);
    }
    return ret;
}

void Core_Object::getSn(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "pdu_version");
    coreItem.sn = getValue(obj, "serialNumber").toString();
    coreItem.mcutemp = getArray(obj, "mcu_temp").toVariantList();
    coreItem.actual.ver.fwVer = getValue(obj, "ver").toString();
    //coreItem.actual.ver.devType = getValue(obj, "dev").toString();
    coreItem.actual.ver.opVers = getArray(obj, "op_vers").toVariantList();
    coreItem.actual.ver.opSn = getArray(obj, "op_sn").toVariantList();
}

void Core_Object::getMac(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "net_addr");
    coreItem.mac = getValue(obj, "mac").toString();
}

void Core_Object::getOutputVol(const QJsonObject &object)
{    
    coreItem.actual.rate.outputVols = getArray(object, "output_vol").toVariantList();
    //cout << coreItem.actual.rate.outputVols.size() << coreItem.actual.rate.outputVols;

    QJsonObject obj = getObject(object, "uut_info");
    coreItem.actual.ver.devType = getValue(obj, "pdu_type").toString();
}

void Core_Object::getAlarmStatus(const QJsonObject &object)
{
    coreItem.alarm = getValue(object, "status").toInt();
}

void Core_Object::getParameter(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    QJsonObject obj = getObject(object, "pdu_info");
    it->devSpec = getData(obj, "pdu_spec");
    it->sensorBoxEn = getData(obj, "sensor_box");
    it->standNeutral = getData(obj, "stand_neutral");
    it->supplyVol = getData(obj, "supply_vol");
    it->lineNum = getData(obj, "line_num");
    it->loopNum = getData(obj, "loop_num");
    it->outputNum = getData(obj, "output_num");
    it->boardNum = getData(obj, "board_num");
    it->language = getData(obj, "language");
    it->isBreaker = getData(obj, "breaker");
    it->vh = getData(obj, "vh");

    sVersion *ver = &coreItem.actual.ver;
    ver->loopOutlets = getArray(obj, "loop_array").toVariantList();
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


void Core_Object::getThreshold(const QJsonObject &object)
{
    sThreshold *it = &coreItem.actual.rate;    
    QJsonObject obj = getObject(object, "line_item_list");
    it->lineVol = getRating(obj, "vol");
    it->lineCur = getRating(obj, "cur");
    it->linePow = getRating(obj, "pow");
    it->volValue = getRating(obj, "vol", "value");

    obj = getObject(object, "loop_item_list");
    it->loopVol = getRating(obj, "vol");
    it->loopCur = getRating(obj, "cur");
    it->loopPow = getRating(obj, "pow");

    obj = getObject(object, "output_item_list");
    it->ops = getArray(obj, "cur_rated").toVariantList();
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


bool Core_Object::checkInput(const QByteArray &msg, QJsonObject &obj)
{
    QJsonParseError jsonerror; bool ret = false;
    QJsonDocument doc = QJsonDocument::fromJson(msg, &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError) {
        if(doc.isObject())  {obj = doc.object(); ret = true;}
    }
    return ret;
}
