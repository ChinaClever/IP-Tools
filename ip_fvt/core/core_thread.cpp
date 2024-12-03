/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#define AD_CUR_RATE 100
#include "dbmacs.h"


Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    http = Core_Http::bulid(this);
    mPro = sDataPacket::bulid()->getPro();
    mDev = sDataPacket::bulid()->getDev();
    mData = &(mDev->data);
    mYc = Yc_Obj::bulid(this);
    mItem = CfgCom::bulid()->item;
    mSocket = new Core_RecvWid(this);

    // connect(mSocket , &Core_RecvWid::msgSig , this ,&Core_Thread::msgSigYC);
    connect(mYc , &Yc_Obj::msgSig , this ,&Core_Thread::msgSigYC);
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
    } else {
        QString ip = m_ips.first();
        for(int i=0; i<5; ++i) {
            ret = cm_pingNet(ip);
            if(ret == true) {
                emit msgSig(tr("设备连接成功"), true);
                break;
            } else cm_mdelay(1*1000);
        }
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
    QFile::remove(fs.first()); http->initHost(ip);
    http->execute("chmod -R 777 /customer/pdu/");
    http->downFile(fs);

    for(int i=0; i<10; ++i) {
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

    if(ret) {
        int rtn = DbMacs::bulid()->contains(m_mac, it.sn);
        if(rtn) { ret = false; emit msgSig(tr("MAC：%1已被分配, 在数据库已存在").arg(m_mac), ret); }
        http->execute("rm /appconfigs/pdu/cfg/proc_cnt.conf");
    }

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

    return res;
}

bool Core_Thread::waitForRest()
{
    http->execute("sync"); cm_mdelay(1000);
    http->execute("killall -9 cores && sleep 0.3 && cores &");
    emit msgSig(tr("设备重启，设备有响声"), true); cm_mdelay(9500);

    bool ret;  QString ip = m_ips.first();
    for(int i=0; i<10; ++i) {
        ret = cm_pingNet(ip);
        if(ret == true) {
            emit msgSig(tr("设备重启成功"), true);
            break;
        } else cm_mdelay(1*1000);
    }

    if(!ret) emit msgSig(tr("设备网络无法连接"), ret);

    return ret;
 }


bool Core_Thread::startCalibration()
{
    bool res = true, ret = waitForRest();
    if(ret) http->calibration(); else return false;
    emit msgSig(tr("校准开始"), true); cm_mdelay(5500);

    QString str = mSocket->return_Recv();
    if(str.isEmpty()) {emit msgSig(tr("校准返回数据超时"), false); return false;}

    QStringList splitList = str.split(';');
    QSet<QString> seenStrings;
    QStringList result;
    for (const QString &str : splitList) {
        if (!seenStrings.contains(str)) {
            seenStrings.insert(str);
            if(!str.isEmpty()) result.append(str);
        }
    }

    if(!result.last().contains("结束",Qt::CaseSensitive))
        res = false;

    // 遍历拆分后的字符串列表，并进行判断
    for (const QString &item : result) {

        if(item.contains("通过",Qt::CaseSensitive))
        {
            ret = true; emit msgSig(item, ret);
        } else if(item.contains("失败",Qt::CaseSensitive) || item.contains("异常",Qt::CaseSensitive))
        {
            ret = false; res = false;
            emit msgSig(item, ret);
        }else {
            emit msgSig(item, true);
        }
    }

    // http->execute("reboot");    //设备重启
    // emit msgSig(tr("设备重启"), true);
    // cm_mdelay(5*1000);

    return res;
}

bool Core_Thread::curErrRange(int exValue, int cur)
{
    bool ret = false;
    int crate = 1;
    int min = (exValue - mItem->curErr * 100)*crate;
    int max = (exValue + mItem->curErr * 100)*crate;
    if((cur >= min) && (cur <= max )) {
        ret =  true;
    }else {
        qDebug() << "cur Err Range" << cur << exValue << mItem->curErr * 100;
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
    int crate = 10.0; double pow = 0;
    if(flag) pow = actual->loopPow[i] * crate;
    else pow = actual->linePow[i] * crate;

    QString str = tr("期望功率%1kW 实际功率 %2kW,第%3位 功率 ").arg(exValue/1000.0).arg(pow/crate).arg(i+1);
    bool ret = powErrRange(exValue, pow *100);
    if(ret) {
        str += tr("正常");
        emit msgSig(str, true);
    } else {
        ret = false; str += tr("错误");
        if(cnt<4){
            emit msgSig(str, true);
        } else {
            emit msgSig(str, ret);
        }
    }

    return ret;
}

bool Core_Thread::curRangeByID(int i, int exValue, int cnt, bool flag)
{
    sThreshold *actual = &coreItem.actual.value;
    QString str ; double cur = 0; bool ret=true;
    if(flag) cur = actual->loopCur[i];
    else cur = actual->lineCur[i];

    str = tr("期望电流%1A: 实际电流 %2A 第%3位 电流 ").arg(exValue/AD_CUR_RATE).arg(cur).arg(i+1);
    ret = curErrRange(exValue, cur*100);

    if(ret) {
        str += tr("正常"); emit msgSig(str, true);
        ret = powRangeByID(i, exValue, cnt, flag);
    } else {
        ret = false; str += tr("错误");
        if(cnt<4){
            emit msgSig(str, true);
        } else {
            emit msgSig(str, ret);
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
        readMetaData();
        ret = curRangeByID(k, exValue, i, flag);
        if(ret) break;
        else cm_mdelay(1*1000);
    }

    return ret;
}

bool Core_Thread::eachCurEnter(int exValue)
{
    bool res = true; int size = 0; bool flag = false;
    sParameter *it = &coreItem.actual.param;
    if(it->loopNum == 0 ) {size = it->lineNum; flag = false;}
    else {size = it->loopNum; flag = true;}

    for(int k=0; k< size; ++k) {
        bool ret = eachCurCheck(k, exValue, flag);
        if(!ret) res = false;
    }

    return res;
}


bool Core_Thread::volErrRangeByID(int i, bool flag)
{
    bool ret = true; double vol = 0;
    sThreshold *actual = &coreItem.actual.value;
    if(flag) vol = actual->loopVol[i];
    else vol = actual->lineVol[i];

    int crate = 1;
    int min = (mItem->vol - mItem->volErr)*crate;
    int max = (mItem->vol + mItem->volErr)*crate;
    QString str = tr("期望电压250V，实际电压%1V 第%2位 电压 ").arg(vol).arg(i+1);
    if((vol >= min) && (vol <= max)) {
        str += tr("正常");
        emit msgSig(str, true);

    } else {
        ret = false; str += tr("错误");
        // emit msgSig(str, false);
        qDebug()<<"volErrRangeByID"<<actual->lineVol[i]<<actual->lineCur[i];
    }

    return ret;
}

bool Core_Thread::volErrRange()
{
    bool res = true; int size = 0;
    bool flag = false; int k = 0;
    sParameter *it = &coreItem.actual.param;
    if(it->loopNum == 0 ) {size = it->lineNum; flag = false;}
    else {size = it->loopNum; flag = true;}

    for(int i=0; i<size; ++i) {
        res = volErrRangeByID(i, flag);
        if(!res) {
            if(k++ < 3){
                i = -1; cm_mdelay(1*1000);
                readMetaData();
            } else {
                res = false;
                QString str = tr("检测到电压 %1 错误").arg(i+1);
                emit msgSig(str, false); break;
            }
            // res = false;
        }
    }

    return res;
}

bool Core_Thread::noLoadCurCheck(int cnt)
{
    bool res = true; int size = 0;
    double cur = 0; double pow = 0; bool flag = true;
    sThreshold *actual = &coreItem.actual.value;
    sParameter *it = &coreItem.actual.param;
    if(it->loopNum == 0 ){size = it->lineNum; flag = false;}
    else {size = it->loopNum; flag = true;}

    for(int k=0; k<size; ++k) {
        QString str = tr("空载校验: 第%1位 ").arg(k+1);
        if(flag) {cur = actual->loopCur[k]; pow = actual->loopPow[k];}
        else {cur = actual->lineCur[k]; pow = actual->linePow[k];}

        if(cur || pow) {
            res = false;
            if(cnt > 3) {
                if(actual->loopCur[k]) str += tr("电流有底数");
                if(actual->loopPow[k]) str += tr("功率有底数");
                emit msgSig(str, res);
            }
        } else {
            str += tr("通过");
            emit msgSig(str, true);
        }
    }

    return res;
}

bool Core_Thread::noLoadCurFun()
{
    bool ret = true;
    for(int i=0; i<4; ++i) {
        QString str = tr("空载校验: 第%1次检查").arg(i+1);
        if(i) emit msgSig(str, true); else cm_mdelay(1*1000);
        readMetaData();
        ret = noLoadCurCheck(i);
        if(ret) break; else {cm_mdelay(1*1000);}
    }

    return ret;
}

bool Core_Thread::startCheck()
{
    bool ret = false;
    mSource = Yc_Obj::bulid()->get();
    mSource->setVol(250, 1);
    ret = mSource->setCur(40, 3);
    cm_mdelay(6*1000);

    QString str = tr("验证电流：期望电流4A");
    emit msgSig(str, ret);

    readMetaData();
    emit msgSig(tr("校验开始"), true);

    if(ret) ret = eachCurEnter(4*AD_CUR_RATE);      //电流、功率校验
    if(ret) ret = mSource->setCur(0, 0);            //标准源电流置为0
    cm_mdelay(5*1000);

    readMetaData();
    if(ret) ret = volErrRange();

    str = tr("空载验证：设置空载电流");
    emit msgSig(str, true);
    if(ret) ret = noLoadCurFun();                   //空载电流验证

    return ret;
}

bool Core_Thread::initFun()
{
    emit msgSig(tr("即将开始"), true);
    bool ret = false;
    ret = mYc->powerOn(60);           //标准源上电---220V，6A
    cm_mdelay(3*1000);

    return ret;
}

void Core_Thread::run()
{
    bool ret = initFun();
    if(ret) ret = searchDev();
    if(ret && fsCheck()) {
        foreach (const auto &ip, m_ips) {
            emit msgSig(tr("目标设备:")+ip, true);
            if(ret) ret = downVer(ip);
            if(ret) timeSync();
            if(ret) ret = workDown(ip);

            if(ret) ret = startCalibration();
            if(ret) ret = startCheck();

            if(ret) enModbusRtu(); //cm_mdelay(150);
            emit finshSig(ret, ip+" ");

            http->execute("reboot");
            mSource = Yc_Obj::bulid()->get();
            mSource->setVol(0, 1); mSource->setCur(0, 1);

#if 0
            cm_mdelay(2000);
            Json_Pack::bulid()->http_post("debugdata/add","192.168.1.12");
#endif
        }m_ips.clear();
    }
    emit overSig();
}
