/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "dtls_sender.h"

bool Dtls_Sender::gRunState = true;
Dtls_Sender::Dtls_Sender(QObject *parent)
    : QObject{parent}
{
    //    mThread = new CThread(this);
    //    mThread->init(this, SLOT(run()));
}

bool Dtls_Sender::writeData(Dtls_Association *dtls)
{    
    QByteArray array; int max=8*1024;
    QString name = dtls->getName();
    bool ret = dtls->writeData(mHead); int pro=0;
    if(ret) emit infoMessage(ret, tr("%1 连接成功").arg(name));
    else {emit errorMessage(name+" Error: Dtls write head"); return ret;}
    for(int i=0; i<mArray.size() && gRunState;i+=array.size()) {
        array = mArray.mid(i, max);
        ret = dtls->writeData(array);
        if(ret) {
            int v = (i*100.0)/mArray.size();
            if(v > pro){pro = v; emit subProgress(name, v);}
        } else {
            emit errorMessage(name+" Error: Dtls write data");
            break;
        }
    }

    return ret;
}

bool Dtls_Sender::workDown(const QString &host)
{
    startNewConnection(host);
    bool ret = mDtls->startHandshake();
    if(ret) {
        ret = writeData(mDtls);
        if(ret && gRunState) emit subProgress(host, 100);
        else if(gRunState) emit infoMessage(ret, tr("%1 Dtls 数据发送失败").arg(host));
    } else emit infoMessage(ret, tr("%1 Dtls 连接失败").arg(host));
    delete mDtls; if(ret) cm_mdelay(1); else cm_mdelay(30);
    return ret;
}

void Dtls_Sender::run()
{
    if(gRunState) {
        QString host = mHost;
        bool ret = workDown(host);
        if(!ret && gRunState) ret = workDown(host);
        if(gRunState) emit finishSig(ret, host);
    }
}

bool Dtls_Sender::sendFile(const QString &ip, const QString &fn, const sOtaFile &it)
{
    bool ret = true; QFile file(fn);
    QByteArray head; QDataStream in(&head, QIODevice::WriteOnly);
    in << it.fc << it.dev << it.path << it.file << it.md5 << it.sig << it.size << END_CRC;
    if(file.exists() && file.open(QIODevice::ReadOnly)) {
        mHost = ip; mHead = head; mArray = file.readAll(); //mThread->onceRun();
    } else {ret = false;} file.close();
    return ret;
}

void Dtls_Sender::sendData(const QString &ip, const sOtaFile &it, const QByteArray &data)
{
    QByteArray head; QDataStream in(&head, QIODevice::WriteOnly);
    in << it.fc << it.dev << it.path << it.file << it.md5 << it.sig << it.size <<END_CRC;
    mHost = ip; mHead = head; mArray = data; //mThread->onceRun();
}

void Dtls_Sender::startNewConnection(const QString &address)
{
    mDtls = new Dtls_Association(address);
    connect(mDtls, &Dtls_Association::errorMessage, this, &Dtls_Sender::throwMessage);
    connect(mDtls, &Dtls_Association::warningMessage, this, &Dtls_Sender::throwMessage);
    connect(mDtls, &Dtls_Association::infoMessage, this, &Dtls_Sender::throwMessage);
    // connect(newConnection.data(), &DtlsAssociation::serverResponse, this, &Dtls_Sender::addServerResponse);
}

void Dtls_Sender::throwMessage(const QString &message)
{
    emit throwSig(message);
}
