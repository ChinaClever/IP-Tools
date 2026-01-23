/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"
#include "print.h"

Json_Pack::Json_Pack(QObject *parent)
{
    mPro = sDataPacket::bulid()->getPro();
}

Json_Pack *Json_Pack::bulid(QObject *parent)
{
    static Json_Pack* sington = NULL;
    if(sington == NULL) {
        sington = new Json_Pack(parent);
    }
    return sington;
}


void Json_Pack::head(QJsonObject &obj)
{
    QDateTime t = QDateTime::currentDateTime();
    mPro->testEndTime = t.toString("yyyy-MM-dd HH:mm:ss");
    mPro->testTime = QString::number(QDateTime::fromString(mPro->testStartTime,"yyyy-MM-dd HH:mm:ss").secsTo(t));
    obj.insert("softwareType", mPro->softwareType);
    obj.insert("productType", mPro->productType);
    obj.insert("moduleSn", mPro->moduleSn);

    obj.insert("productSN",mPro->productSN);

    obj.insert("macAddress", mPro->macAddress);
    obj.insert("result", mPro->uploadPassResult);
    obj.insert("languageSelect",0);
    obj.insert("softVersion", mPro->softwareVersion);
    obj.insert("companyName", mPro->companyName);
    obj.insert("protocolVersion", mPro->protocolVersion);
    obj.insert("testStartTime", mPro->testStartTime);
    obj.insert("toolName","Ip_qc");
    //qDebug()<<mPro->testStartTime<<" "<<mPro->testEndTime<<"   ?";
    obj.insert("testEndTime", mPro->testEndTime);
    obj.insert("testTime", mPro->testTime);
    obj.insert("orderId", mPro->pn);
    obj.insert("orderNum", mPro->orderNum);

    //pduInfo(obj);
}

void Json_Pack::pduInfo(QJsonObject &obj,const int id)
{
    objData(obj,id);
}



int Json_Pack::objData(QJsonObject &Obj,const int id)
{
    QJsonArray jsonArray;
    int num = mPro->uploadPass.size();

    for(int i=0; i<num; ++i)
    {
        QJsonObject subObj;
        subObj.insert("no",QString::number(i+1));
        subObj.insert("testProcess", mPro->itemName.at(i));
        subObj.insert("testResult", mPro->uploadPass.at(i)?1:0);

        subObj.insert("testRequest", mPro->testRequest.at(i));
        subObj.insert("testStep", mPro->testStep.at(i));
        subObj.insert("testItem", mPro->testItem.at(i));
      //  qDebug()<<subObj;
        jsonArray.append(subObj);
    }
    Obj.insert("testData" ,QJsonValue(jsonArray));

    return num;
}

void Json_Pack::http_post(const QString &method, QJsonObject &json, const int id, const QString &ip, int port)
{
    //QJsonObject json; head(json);
    pduInfo(json,id);

    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();
    QString url = "http://%1:%2/%3";
    http.post(url.arg(ip).arg(port).arg(method))
        .header("content-type", "application/json")
        .onSuccess([&](QString result) {emit httpSig(result,true,"","","");})
        .onFailed([&](QString error) {cout << error;emit httpSig(error,false,"","",""); })
        .onTimeout([&](QNetworkReply *) {cout << "http_post timeout";emit httpSig("http_post timeout",false,"","",""); }) // 超时处理
        .timeout(1000) // 1s超时
        .block()
        .body(json)
        .exec();
}

//bool Json_Build::saveJson( QJsonObject &json)
//{
//    QJsonDocument jsonDoc(json);
//    QByteArray ba = jsonDoc.toJson();
//    QString path = CfgCom::bulid()->pathOfData("pdu_id.json");
//    QFile file(path);
//    bool ret = false;
//    if(file.exists())//文件存在则不需要再写
//    {
//        ret = true;
//    }
//    else//文件不存在则写入初始数据
//    {
//        ret = file.open(QIODevice::WriteOnly);
//        if(ret) {
//            file.write(ba);
//            file.close();
//        } else {
//            qDebug() << "write json file failed";
//        }
//    }

//    return ret;
//}

