/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */

#include "ssdp_setting.h"

Ssdp_Setting::Ssdp_Setting(QObject *parent) : Ssdp_Search(parent)
{
     udpBind();
}


bool Ssdp_Setting::setting(const sDataItem &it, const QString &room, const QString &ip)
{
    QJsonObject json;
    json.insert("addr", it.addr);
    json.insert("type", it.type);
    json.insert("topic", it.topic);
    json.insert("subtopic", it.subtopic);
    json.insert("value", it.value/1.0);
    json.insert("id", it.id);

    QJsonObject obj;
    obj.insert("pduDataSet", json);
    QJsonDocument doc(obj);
    return setJson(doc.toJson(QJsonDocument::Compact), room, ip);
}

bool Ssdp_Setting::setCfg(const sCfgItem &it, const QVariant &v, const QString &room, const QString &ip)
{
    QJsonObject json;
    json.insert("addr", it.addr);
    json.insert("type", it.type);
    json.insert("fc", it.fc);
    json.insert("id", it.id);
    json.insert("value", v.toJsonValue());

    QJsonObject obj;
    obj.insert("pduCfgSet", json);
    QJsonDocument doc(obj);
    return setJson(doc.toJson(QJsonDocument::Compact), room, ip);
}

bool Ssdp_Setting::setJson(const QByteArray &data, const QString &room, const QString &ip)
{
    sSdpIt item; item.fc = 1; item.data = data;
    item.room = room; item.ip = ip;

    QString msg;
    if(ip.size()) msg += "IP:" + ip;
    if(room.size()) msg += "room:" + room;
    msg += "data:" + data; emit sendMsgSig(msg);
    emit sendDataSig(data);

    return ssdpSend(item);
}

bool Ssdp_Setting::swVersion(const QString &room, const QString &ip)
{
    sSdpIt item; item.fc = 11;
    item.room = room; item.ip = ip;

    QString msg;
    if(ip.size()) msg += "IP:" + ip;
    if(room.size()) msg += "room:" + room;
    msg += "sw version"; emit sendMsgSig(msg);
    //emit sendDataSig(data);

    return ssdpSend(item);
}
