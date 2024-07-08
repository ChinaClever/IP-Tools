/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_sender.h"

Core_Sender::Core_Sender(QObject *parent)
    : QThread{parent}
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutDone()));
    int cnt = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(4*cnt);
}

Core_Sender *Core_Sender::bulid(QObject *parent)
{
    static Core_Sender* sington = nullptr;
    if(sington == nullptr) {
        sington = new Core_Sender(parent);
    }
    return sington;
}

bool Core_Sender::sendFile(const QStringList &ips, const QString &fn, const sOtaFile &it)
{
    bool ret = isRun = true; emit devListSig(ips);
    mHosts = ips; mFileTrans = it; QFile file(fn);
    if(file.exists() && file.open(QIODevice::ReadOnly)) {
        Dtls_Sender::setRunState(isRun);
        mArray = file.readAll();
        timer->start(200);
        run(); //start();
    } else ret = false;
    file.close();
    return ret;
}

void Core_Sender::initSender(Dtls_Sender *sender)
{
    connect(sender, &Dtls_Sender::errorMessage, this, &Core_Sender::errorMessage);
    connect(sender, &Dtls_Sender::subProgress, this, &Core_Sender::subProgress);
    connect(sender, &Dtls_Sender::infoMessage, this, &Core_Sender::infoMessage);
    connect(sender, &Dtls_Sender::finishSig, this, &Core_Sender::finishSig);
    connect(sender, &Dtls_Sender::throwSig, this, &Core_Sender::throwSig);
}

void Core_Sender::appendSender()
{
    QString host = mHosts.takeFirst();
    Dtls_Sender *sender = new Dtls_Sender();
    sender->sendData(host, mFileTrans, mArray);
    initSender(sender); cm_mdelay(1);
    QThreadPool::globalInstance()->start(sender);
}

void Core_Sender::run()
{
    while(mHosts.size() && isRun) {
        int cnt = QThreadPool::globalInstance()->maxThreadCount();
        for(int i=0; i<cnt && mHosts.size(); ++i) appendSender();
    }
}


void Core_Sender::timeoutDone()
{
    if(isFinsh()) {
        emit overSig();
        timer->stop();
    }
}
