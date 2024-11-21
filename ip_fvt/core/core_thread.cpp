/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#define AD_CUR_RATE 100

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    http = Core_Http::bulid(this);
    http->initListen();
    mPro = sDataPacket::bulid()->getPro();
    mDev = sDataPacket::bulid()->getDev();
    mData = &(mDev->data);
    mYc = Yc_Obj::bulid(this);
    mItem = CfgCom::bulid()->item;
    connect(this, &Core_Thread::startSig, &Core_Thread::startSlot);

}

QStringList Core_Thread::getFs()
{
    QString dir = "customer/pdu/"; FileMgr::build().mkpath(dir);
    dir = "appconfigs/pdu/doc"; FileMgr::build().mkpath(dir);
    dir = "appconfigs/pdu/cfg/"; FileMgr::build().mkpath(dir);

    QStringList fs; fs << "customer/pdu/ver.ini" << "appconfigs/pdu/doc/modbus.xlsx";
    fs << dir+"alarm.conf" << dir+"snmpd.conf" << dir+"devParam.ini" << dir+"inet.ini" ;
    fs << dir+"alarm.cfg" << dir+"logo.png" << dir+"cfg.ini" ;
    fs << dir+"mac.conf" << dir+"sn.conf" ;
    return fs;
}

bool Core_Thread::fsCheck()
{
    bool ret = true;
    QStringList fs = getFs();
    fs.removeLast(); fs.removeLast();
    foreach (const auto fn, fs) {
        if(!QFile::exists(fn)) {
            ret = false;
            emit msgSig(tr("文件未找到")+fn, ret);
        }
        int size = File::fileSize(fn);
        if(!size) {
            ret = false;
            emit msgSig(tr("文件为空")+fn, ret);
        }
    } if(ret) emit msgSig(tr("文件未检查 OK!"), ret);
    return ret;
}


bool Core_Thread::searchDev()
{
    bool ret = true; if(m_ips.isEmpty()) {
        Ssdp_Core *ssdp = Ssdp_Core::bulid(this);
        QStringList ips = ssdp->searchAll();
        if(ips.isEmpty()) ips = ssdp->searchAll();
        QString str = tr("未找到任何目标设备"); // cm_mdelay(150);
        if(ips.size()) str = tr("已找到%1个设备").arg(ips.size());
        else {ret = false;} m_ips = ips;
        emit msgSig(str, ret);
    }
    return ret;
}

void Core_Thread::readMetaData()
{
    http->readJson(0);
    //cm_mdelay(455);
    jsonAnalysis();
}

void Core_Thread::timeSync()
{
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QString t = QDateTime::currentDateTime().toString(fmd);
    emit msgSig(tr("时间设置：")+t, true);
    sCfgItem it; it.type = 43; it.fc =1;
    http->setting(it, t); cm_mdelay(321);
    it.type = 30; it.fc = 9;
    http->setting(it, t);
}


void Core_Thread::enModbusRtu()
{
    sCfgItem it; it.type = 15; it.fc = 1;
    http->setting(it, 1);
    it.fc = 7; http->setting(it, 1);
    emit msgSig(tr("设备模式：已开启Modbus-RTU功能"), true);

    //it.type = 13; it.fc = 3;
    //Core_Http::bulid(this)->setting(it, 1);
    //emit msgSig(tr("设备模式：已开启设备级联功能"), true);

    //it.type = 13; it.fc = 9;
    //Core_Http::bulid(this)->setting(it, 1);
    //emit msgSig(tr("启用扩展口：已开启传感器盒子功能"), true);
}

void Core_Thread::writeSnMac(const QString &sn, const QString &mac)
{
    QString dir = "appconfigs/pdu/cfg/"; QFile file(dir + "mac.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write(mac.toLatin1());
    } file.close();

    file.setFileName(dir + "sn.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        file.write(sn.toLatin1());
    } file.close();
}


bool Core_Thread::downVer(const QString &ip)
{
    QString str = tr("下载版本信息:");
    QStringList fs; fs << "customer/pdu/ver.ini";
    QFile::remove(fs.first());
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(fs.first())) break; else cm_mdelay(100);
    } QString dir = "customer/pdu";
    Cfg_App cfg(dir, this); sAppVerIt it;
    bool ret = cfg.app_unpack(it);
    if(ret) {
        it.sn = m_sn = createSn();
        mPro->productSN = m_sn;
        cfg.app_serialNumber(it.sn);
        QString mac = m_mac = updateMacAddr();
        writeSnMac(it.sn, mac); //str += "ok\n";
        str = "SN：" + m_sn + "   MAC：" + m_mac;
    } else str =  tr("版本信息读取错误");
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::workDown(const QString &ip)
{    
    QStringList fs = getFs(); bool res = true;
    foreach (const auto fn, fs) {
        bool ret = http->uploadFile(fn);
        if(!ret) res = false;
        emit msgSig(fn, ret);
        cm_mdelay(20);
    }

    if(res) {
        emit msgSig(tr("设备重启，设备有响声"), true);        
        http->execute("sync"); //cm_mdelay(1000);
        http->execute("reboot"); // killall cores
    }
    return res;
}

bool Core_Thread::startCalibration()
{
    bool ret = false, res = true;

    http->calibration(); cm_mdelay(20);

    QString str = http->backListenValue();
    QStringList splitList = str.split(",");

    // 遍历拆分后的字符串列表，并进行判断
    for (const QString &item : splitList) {

        if(item.contains("成功",Qt::CaseSensitive))
        {
            ret = true; emit msgSig(item, ret);

        } else if(item.contains("失败",Qt::CaseSensitive))
        {
            ret = false; res = true;
            emit msgSig(item, ret);
        }
    }

    http->execute("reboot");    //设备重启

    return res;
}

bool Core_Thread::curErrRange(int exValue, int cur)
{
    bool ret = false;
    int crate = 1;
    int min = (exValue - mItem->curErr * 10)*crate;
    int max = (exValue + mItem->curErr * 10)*crate;
    if((cur >= min) && (cur <= max )) {
        ret =  true;
    }

    return ret;
}

bool Core_Thread::powErrRange(int exValue, int pow)
{
    bool ret = false;
    int err = exValue * mItem->powErr/1000.0;
    int min = exValue - err;
    int max = exValue + err;
    if((pow >= min) && (pow <= max )) {
        ret =  true;
    } else {
        qDebug() << "pow Err Range" << pow << exValue << err;
    }

    return ret;
}

bool Core_Thread::powRangeByID(int i, int exValue, int cnt, bool flag)
{
    sThreshold *actual = &coreItem.actual.value;
    exValue = mItem->vol * exValue/AD_CUR_RATE; //mData->cur.value[i]/COM_RATE_CUR;
    exValue *= 0.5;
    QString str = tr("期望功率%1kW 第%2位 功率 ").arg(exValue/1000.0).arg(i+1);
    int crate = 1; int pow = 0;
    if(flag) pow = actual->loopPow[i] * crate ;
    else pow = actual->linePow[i] * crate;

    bool ret = powErrRange(exValue, pow);
    if(ret) {
        str += tr("正常");
        emit msgSig(str, true);
    } else {
        ret = false;
        if(cnt > 3) {
            str += tr("错误");
            emit msgSig(str, false);
        }
    }

    return ret;
}

bool Core_Thread::curRangeByID(int i, int exValue, int cnt, bool flag)
{
    sThreshold *actual = &coreItem.actual.value;
    QString str ; int cur = 0;
    bool ret=true, res=true;
    if(flag) cur = actual->loopCur[i] * 10 ;
    else cur = actual->lineCur[i] * 10;

    str = tr("期望电流%1A: 实际电流 %2A 第%3位 电流 ").arg(exValue/AD_CUR_RATE).arg(mData->cur.value[i]/COM_RATE_CUR).arg(i+1);
    ret = curErrRange(exValue, cur);

    if(ret) {
        ret = powRangeByID(i, exValue, cnt, flag);
        if(ret){str += tr("正常"); emit msgSig(str, true);}
    } else {
        ret = false;
        if(cnt > 3) {
            str += tr("错误");
            emit msgSig(str, false);
        }
    }

    return ret;
}


bool Core_Thread::eachCurCheck(int k, int exValue, bool flag)
{
    bool ret = true;
    double value = mItem->vol*exValue/AD_CUR_RATE/1000.0 * 0.5;
    QString str = tr("校验数据: 期望电流%1A 功率%2kW").arg(exValue/AD_CUR_RATE).arg(value);
    emit msgSig(str, true);

    for(int i=0; i<5; ++i) {
        if(i) str += tr(" 第%1次").arg(i+1); //else delay(4);
        ret = curRangeByID(k, exValue, i, flag);
        if(ret) break;
        else cm_mdelay(i+5);
        readMetaData();
    }

    return ret;
}

bool Core_Thread::eachCurEnter(int exValue)
{
    bool res = true; int size = 0; bool flag = false;
    readMetaData();
    sParameter *it = &coreItem.actual.param;
    if(it->loopNum == 0 ) { size = it->lineNum; flag = false;}
    else {size = it->loopNum; flag = true;}

    for(int k=0; k< size; ++k) {
        bool ret = eachCurCheck(k, exValue, flag);
        if(!ret) res = false;
    }

    return res;
}



bool Core_Thread::startCheck()
{
    emit msgSig(tr("校验开始"), true);
    bool ret = false;

    mSource = Yc_Obj::bulid()->get();
    mSource->setVol(250, 1);
    ret = mSource->setCur(40, 5);
    QString str = tr("验证电流：期望电流4A");
    emit msgSig(str, ret);

    ret = eachCurEnter(4*AD_CUR_RATE);

    return ret;
}

bool Core_Thread::initFun()
{
    emit msgSig(tr("即将开始"), true);
    bool ret = false;
    ret = mYc->powerOn();

    return ret;
}

void Core_Thread::run()
{
    eachCurCheck(3, 400, 1);
    bool ret = searchDev();
    if(ret && fsCheck()) {
        foreach (const auto &ip, m_ips) {            
            emit msgSig(tr("目标设备:")+ip, true);

            ret = initFun();            //标准源上电---220V，6A

            if(ret) ret = downVer(ip);
            if(ret) timeSync();
            if(ret) ret = workDown(ip);

            if(ret) ret = startCalibration();

            // if(ret) ret = startCheck();

            if(ret) enModbusRtu(); //cm_mdelay(150);
            emit finshSig(ret, ip+" ");
#if 0
            cm_mdelay(2000);
            Json_Pack::bulid()->http_post("debugdata/add","192.168.1.12");
#endif
        }m_ips.clear();
    } emit overSig();
}
