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
    mModbus = Rtu_Modbus::bulid(this)->get();
    // setModbus();
}

void Core_Object::setModbus()
{
     mModbus = Rtu_Modbus::bulid(this)->get();
}

void Core_Object::clearAllEle()
{
    sDataItem it;
    it.id = 0;
    it.type = 1;
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
    it->jsonPacket.clear(); bool ret = false;
    if(!msg.isEmpty())
    {
        ret = checkInput(msg, obj);
        if(ret) {
            getSn(obj); getMac(obj); getDevType(obj);
            getParameter(obj); getAlarmStatus(obj);
            it->actual.datetime = getValue(obj, "datetime").toString();
            obj = getObject(obj, "pdu_data"); getThreshold(obj);
            getPduData(obj); getTgData(obj); getEnvData(obj);
        }
    }

    return ret;
}

bool Core_Object::jsonAnalysisRefer()
{
    QJsonObject obj; sCoreItem *it = &coreItem;
    bool ret = false;
    if(it->jsonPacket.isEmpty()) return ret;
    QByteArray msg = it->jsonPacket.toLatin1();
    it->jsonPacket.clear();
    if(!msg.isEmpty())
    {
        ret = checkInput(msg, obj);
        if(ret) {
            // getSnRefer(obj); getMacRefer(obj); getDevTypeRefer(obj);
            // getParameterRefer(obj); getAlarmStatusRefer(obj);
            // it->desire.datetime = getValue(obj, "datetime").toString();
            obj = getObject(obj, "pdu_data"); //getThresholdRefer(obj);
            getPduDataRefer(obj); getTgDataRefer(obj); getEnvDataRefer(obj);
        }
    }
    return ret;
}

void Core_Object::getSn(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "pdu_version");
    coreItem.actual.sn = getValue(obj, "serialNumber").toString();
    coreItem.actual.ver.fwVer = getValue(obj, "ver").toString();
    coreItem.actual.ver.devType = getValue(obj, "dev").toString();
}

void Core_Object::getMac(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "net_addr");
    coreItem.actual.mac = getValue(obj, "mac").toString();
}

void Core_Object::getAlarmStatus(const QJsonObject &object)
{
    coreItem.actual.alarm = getValue(object, "status").toInt();
}

void Core_Object::getDevType(const QJsonObject &object)
{
    QJsonObject obj = getObject(object, "uut_info");
    coreItem.actual.ver.devType = getValue(obj, "pdu_type").toString();
}

void Core_Object::getParameter(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    QJsonObject obj = getObject(object, "pdu_info");
    it->devSpec = getData(obj, "pdu_spec");
    it->oldProtocol = getData(obj, "old_protocol");
    it->standNeutral = getData(obj, "stand_neutral");
    it->lineNum = getData(obj, "line_num");
    it->loopNum = getData(obj, "loop_num");
    it->language = getData(obj, "language");
    it->isBreaker = getData(obj, "breaker");
    it->vh = getData(obj, "vh");
    it->cpuTem = getData(obj, "cpu_temp");
}

void Core_Object::getParameterRefer(const QJsonObject &object)
{
    sParameter *it = &coreItem.desire.param;
    QJsonObject obj = getObject(object, "pdu_info");
    it->devSpec = getData(obj, "pdu_spec");
    it->oldProtocol = getData(obj, "old_protocol");
    it->standNeutral = getData(obj, "stand_neutral");
    it->lineNum = getData(obj, "line_num");
    it->loopNum = getData(obj, "loop_num");
    it->language = getData(obj, "language");
    it->isBreaker = getData(obj, "breaker");
    it->vh = getData(obj, "vh");
    it->cpuTem = getData(obj, "cpu_temp");
}

void Core_Object::getTgData(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.actual.data;
    QJsonObject obj = getObject(object, "pdu_total_data");
    it->apparent_pow = getData(obj, "pow_apparent");
    it->active_pow = getData(obj, "pow_active");
    it->reactive_pow = getData(obj, "pow_reactive");
    it->tg_ele = getData(obj, "ele_active");
    it->tg_reactiveEle = getData(obj, "ele_reactive");
    it->tg_apparentEle = getData(obj, "ele_apparent");
}

void Core_Object::getTgDataRefer(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.desire.data;
    QJsonObject obj = getObject(object, "pdu_total_data");
    it->apparent_pow = getData(obj, "pow_apparent");
    it->active_pow = getData(obj, "pow_active");
    it->reactive_pow = getData(obj, "pow_reactive");
    it->tg_ele = getData(obj, "ele_active");
    it->tg_reactiveEle = getData(obj, "ele_reactive");
    it->tg_apparentEle = getData(obj, "ele_apparent");
}

void Core_Object::getEnvData(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.actual.data;
    QJsonObject obj = getObject(object, "env_item_list");
    it->doors = getArray(obj, "door").toVariantList();
    it->temps = getArray(obj, "tem_value").toVariantList();
}

void Core_Object::getEnvDataRefer(const QJsonObject &object)
{
    sMonitorData *it = &coreItem.desire.data;
    QJsonObject obj = getObject(object, "env_item_list");
    it->doors = getArray(obj, "door").toVariantList();
    it->temps = getArray(obj, "tem_value").toVariantList();
}
void Core_Object::getPduData(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    sPdudata *item = &coreItem.actual.value;
    QJsonObject obj; int loop = (int)it->loopNum;

    if(loop)
    {
        obj = getObject(object, "loop_item_list");
        item->loopVol = getArray(obj, "vol_value").toVariantList();
        item->loopCur = getArray(obj, "cur_value").toVariantList();
        item->loopPow = getArray(obj, "pow_value").toVariantList();
        item->loopPF = getArray(obj, "power_factor").toVariantList();
        item->loopEle = getArray(obj, "ele_active").toVariantList();
    }

    obj = getObject(object, "line_item_list");
    item->lineVol = getArray(obj, "vol_value").toVariantList();
    item->lineCur = getArray(obj, "cur_value").toVariantList();
    item->linePow = getArray(obj, "pow_value").toVariantList();
    item->linePF = getArray(obj, "power_factor").toVariantList();
    item->lineEle = getArray(obj, "ele_active").toVariantList();
}

void Core_Object::getPduDataRefer(const QJsonObject &object)
{
    sParameter *it = &coreItem.desire.param;
    sPdudata *item = &coreItem.desire.value;
    QJsonObject obj; int loop = (int)it->loopNum;

    if(loop) {
        obj = getObject(object, "loop_item_list");
        item->loopVol = getArray(obj, "vol_value").toVariantList();
        item->loopCur = getArray(obj, "cur_value").toVariantList();
        item->loopPow = getArray(obj, "pow_value").toVariantList();
    }

    obj = getObject(object, "line_item_list");
    item->lineVol = getArray(obj, "vol_value").toVariantList();
    item->lineCur = getArray(obj, "cur_value").toVariantList();
    item->linePow = getArray(obj, "pow_value").toVariantList();
}

void Core_Object::getThreshold(const QJsonObject &object)
{
    sParameter *it = &coreItem.actual.param;
    sThreshold *item = &coreItem.actual.rate;
    QJsonObject obj; int loop = (int)it->loopNum;

    if(loop) {
        obj = getObject(object, "loop_item_list");
        item->loopVol = getArray(obj, "vol_rated").toVariantList();
        item->loopCur = getArray(obj, "cur_rated").toVariantList();
        item->loopPow = getArray(obj, "pow_rated").toVariantList();
    }

    obj = getObject(object, "line_item_list");
    item->lineVol = getArray(obj, "vol_rated").toVariantList();
    item->lineCur = getArray(obj, "cur_rated").toVariantList();
    item->linePow = getArray(obj, "pow_rated").toVariantList();
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

double Core_Object::getRating(const QJsonObject &object, const QString &key,  int value, const QString &suffix)
{
    QJsonArray array = getArray(object, key+"_"+suffix);
    if(suffix.contains("rated")) {
        for (int i = 1; i < array.size(); ++i) {
            if (i == value) {
                break;
            }
        }
    }

    return array.at(value).toDouble();
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
