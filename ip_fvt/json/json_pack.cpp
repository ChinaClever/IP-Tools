/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"


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
    obj.insert("moduleType", mPro->productType);
    obj.insert("moduleSn", mPro->moduleSn);
    obj.insert("toolName","Ip_fvt");
    obj.insert("languageSelect",0);
    obj.insert("productSn", mPro->productSN);
    obj.insert("macAddress", mPro->macAddress);
    obj.insert("result", mPro->uploadPassResult);
    obj.insert("softVersion", mPro->softwareVersion);
    obj.insert("companyName", mPro->companyName);
    obj.insert("protocolVersion", mPro->protocolVersion);
    obj.insert("testStartTime", mPro->testStartTime);
    obj.insert("testEndTime", mPro->testEndTime);
    obj.insert("testTime", mPro->testTime);
    obj.insert("work_order", mPro->pn);

    obj.insert("orderId", mPro->pn);
    obj.insert("orderNum", mPro->orderNum);


   // pduInfo(obj,id);;
}

// void Json_Pack::part_head(QJsonObject &obj)
// {
//     obj.insert("work_order", "mPro->pn");
//     obj.insert("serial_id", mPro->productSN);

// }
void Json_Pack::part_head(QJsonObject &obj)
{
    obj.insert("serial_id", mPro->moduleSn);
    obj.insert("work_order", mPro->pn);


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

        jsonArray.append(subObj);
    }
    Obj.insert("testData" ,QJsonValue(jsonArray));

    return num;
}

void Json_Pack::http_post(const QString &method, QJsonObject &json, const int id,const QString &ip,  int port)
{
    //QJsonObject json; head(json);

    pduInfo(json,id);

    AeaQt::HttpClient http;
    http.clearAccessCache();
    http.clearConnectionCache();

    QString url = "http://%1:%2/%3";

    http.post(url.arg(ip).arg(port).arg(method))
        .header("content-type", "application/json")
        .onSuccess([&](QString result) {qDebug() <<1223; emit httpSig(result,true);})
        .onFailed([&](QString error) {qDebug() <<12233;emit httpSig(error,false); })
        .onTimeout([&](QNetworkReply *) {emit httpSig("http_post timeout",false); }) // 超时处理
        .timeout(1000) // 1s超时
        .block()
        .body(json)
        .exec();

}

QString Json_Pack::http_get(const QString &method, const QString &ip, QJsonObject &obj, int port)
{
    QString macResult; // 1. 外部变量准备接收值
    AeaQt::HttpClient http;

    // 自动将 json 转为 URL 参数
    QUrlQuery query;
    for (QString key : obj.keys()) {
        query.addQueryItem(key, obj.value(key).toVariant().toString());
    }

    QString url = QString("http://%1:%2/%3?%4").arg(ip).arg(port).arg(method).arg(query.toString());

    http.get(url)
        .header("content-type", "application/json")
        .onSuccess([&](QString result) {
            // 关键：这里直接赋值！不再使用 QJsonDocument 解析
            qDebug() << "Server Reply (Success):" << result;
            macResult = result.trimmed(); // 去除两侧空格或换行符
            // 移除引号（如果返回结果自带引号，如 ""00:A0:..."")
            macResult.remove('\"');
        })
        .onFailed([&](QString error) {
            qDebug() << "HTTP Failed:" << error;
        })
        .timeout(2000)
        .block() // 必须有 block() 才能让函数停在这等结果
        .exec();

    return macResult; // 这里返回后，temp 就不再是空的了
}
// void Json_Pack::http_post_resve()
// {
//     qDebug() << "3------Mac Addr Err";

//     QNetworkRequest request;
//     QNetworkAccessManager * naManager = new QNetworkAccessManager(this);
//     connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
//     request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
//     request.setUrl(QUrl::fromUserInput(" http://localhost:8000/mac/test"));

//     QJsonObject json; part_head(json);
//     QJsonDocument jsonDoc(json);
//     QByteArray ba = jsonDoc.toJson();
//     QNetworkReply* reply = naManager->post(request,ba);//发起post请求
// }
// //接收响应槽函数
// void Json_Pack::requestFinished(QNetworkReply *reply)
// {
//     qDebug() << "4------Mac Addr Err";
//     QString loginResultJson;
//     QByteArray bytes = reply->readAll();
//     qDebug() << "bytes:   " << QString::fromStdString(bytes.toStdString());
//     // 获取http状态码
//     QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//     if(statusCode.isValid())
//     {
//         qDebug() << "status code=" << statusCode.toInt();
//     }

//     QNetworkReply::NetworkError err = reply->error();
//     if(err != QNetworkReply::NoError)
//     {
//         qDebug() << "Failed: " << reply->errorString();
//     }
//     else
//     {
//         // 获取返回内容
//         qDebug() << "loginResultJson  is  " <<  QString::fromStdString(bytes.toStdString());
//         loginResultJson =  QString::fromStdString(bytes.toStdString());
//         //处理返回数据
//         MacAddr::bulid()->macItem->mac = loginResultJson;
//     }
// }


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

