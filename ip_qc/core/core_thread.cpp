/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#include <QImage>
#include <QPixmap>
#include "dbmacs.h"

#define RATED 10000.0

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    mLogo = "usr/data/pdu/cfg/logo.png";
    QString dir = "usr/data/pdu/cfg";
    FileMgr::build().mkpath(dir);
    mItem = CfgCom::bulid()->item;
    http = Core_Http::bulid(this);

    dir = "etc/ssl/certs";
    FileMgr::build().mkpath(dir);
    mTls = "etc/ssl/certs/client-cert.pem";
}

Core_Thread *Core_Thread::bulid(QObject *parent)
{
    static Core_Thread* sington = nullptr;
    if(!sington) sington = new Core_Thread(parent);
    return sington;
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
        emit msgSig(str, ret); //cout << ips;
    }else {
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


bool Core_Thread::timeCheck()
{
    bool ret = false;
    QString str = tr("设备时间，");
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QDateTime dt = QDateTime::fromString(coreItem.actual.datetime, fmd);
    QDateTime time = QDateTime::currentDateTime();
    int secs = qAbs(time.secsTo(dt));
    cout << dt << time;
    if(secs > 10) str += tr("相差过大："); else ret = true;
    str += coreItem.actual.datetime;
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::snCheck()
{
    bool ret = false;
    QString sn = coreItem.actual.sn;
    QString str = tr("序列号：");
    if(sn.contains("3I5")) {str += sn; ret = true;}
    else {str += tr("错误：sn=%1").arg(sn);}

    QString uuid = coreItem.actual.uuid;
    emit msgSig(str, ret); if(ret) {
        if(mHashSn.contains(sn)) {
            if(mHashSn.value(sn) != uuid) {
                ret = false; emit msgSig(tr("SN：%1已被分配给UUID:%2")
                                .arg(sn, uuid), ret);
            }
        } else mHashSn[sn] = uuid;
    }
    return ret;
}

bool Core_Thread::macCheck()
{
    bool ret = false;
    QString v = coreItem.actual.mac;
    QString str = tr("MAC地址：");
    if(v.contains("2C:26:")) {str += v; ret = true;}
    else {str += tr("错误：mac=%1").arg(v);}

    QString sn = coreItem.actual.sn;
    QString uuid = coreItem.actual.uuid;
    emit msgSig(str, ret); if(ret) {
        if(mHashMac.contains(v)) {
            if(mHashMac.value(v) != uuid) {
                ret = false; emit msgSig(tr("MAC：%1已被分配给SN:%3 UUID:%2")
                                .arg(v, uuid, mHashMac.value(v)), ret);
            }
        } else {
            int rtn = DbMacs::bulid()->contains(v, sn);
            if(rtn) { ret = false; emit msgSig(tr("MAC：%1已被分配, 在数据库有"), ret); }
            else mHashMac[v] = uuid;
        }
    }

    return ret;
}

bool Core_Thread::downLogo(const QString &ip)
{
    QString str = tr("Logo图片下载：");
    QStringList fs; fs << mLogo;

    QFile::remove(mLogo);
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(mLogo)) break; else cm_mdelay(100);
    }

    bool ret =QFile::exists(mLogo);
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret);
    return ret;
}

// 计算两张图片的差异
bool Core_Thread::compareImages()
{
    if(File::fileSize(mLogo) != File::fileSize(coreItem.logo))  return false;

    // 加载两个图片文件
    QImage img1(mLogo);
    QImage img2(coreItem.logo);

    // 将两张图片转换为QImage类型，以便进行像素比较
    //QImage img1 = pix1.toImage();
    //QImage img2 = pix2.toImage();

    // 检查图片尺寸是否相同
    if (img1.size() != img2.size()) {
        return false;
    }

    // 计算不同像素点的数量
    uint diffCount = 0; uint count = 0;
    for (int y = 0; y < img1.height(); ++y) {
        for (int x = 0; x < img1.width(); ++x) {
            if (img1.pixelColor(x, y) != img2.pixelColor(x, y)) {
                ++diffCount;
            } ++count;
        }
    }

    // 设置一个阈值，判断两张图片是否大致相同
    const double threshold = 0.1; // 根据实际情况调整阈值
    if ((diffCount*1.0)/count <= threshold) {
        return true;
    } else {
        return false;
    }
}

bool Core_Thread::logoCheck(const QString &ip)
{
    bool ret = true;
    if(coreItem.desire.param.standNeutral<2) {
        emit msgSig(tr("标准和中性版本跳过Logo检测"), true);
    } else if (coreItem.logo.isEmpty()) {
         emit msgSig(tr("未指定Logo此项检测跳过"), true);
     } else {
        ret = downLogo(ip);
        if(ret) {
            ret = compareImages();
            QString str = tr("Logo图片差异检查：");
            if(ret) str += tr("通过"); else str += tr("失败");
            emit msgSig(str, ret);
        }
    }

    return ret;
 }

bool Core_Thread::downTlsCert(const QString &ip)
{
    QString str = tr("Tls证书下载：");
    QStringList fs; fs << mTls;

    QFile::remove(mTls);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(mTls)) break; else cm_mdelay(100);
    }

    bool ret = QFile::exists(mTls);
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::compareTls()
{
    if(File::fileSize(mTls) != File::fileSize(coreItem.tlsFile))  return false;
    return File::md5(mTls) == File::md5(coreItem.tlsFile);
}

bool Core_Thread::tlsCertCheck(const QString &ip)
{
    bool ret = true;
    if(coreItem.desire.param.standNeutral) {
        if(coreItem.tlsFile.size()) {
            ret = downTlsCert(ip);
            if(ret) {
                ret = compareTls();
                QString str = tr("证书差异检查：");
                if(ret) str += tr("通过"); else str += tr("失败");
                emit msgSig(str, ret);
            }

        } else emit msgSig(tr("未指定证书此项检测跳过"), true);
    }

    return ret;

}


QString Core_Thread::changeSpec(int value)
{
    QString str;
    switch(value){
    case 0:
        str = tr("标准"); break;
    case 1:
        str = tr("中性"); break;
    case 2:
        str = tr("定制"); break;
    }
    return str;
}

QString Core_Thread::changeType(int value)
{
    QString str;
    switch(value){
    case 0:
        str = tr("互感器"); break;
    case 1:
        str = tr("32A小锰铜"); break;
    case 2:
        str = tr("直流"); break;
    }
    return str;
}

QString Core_Thread::changeProtocol(int value)
{
    QString str;
    switch(value){
    case 0:
        str = tr("标准"); break;
    case 1:
        str = tr("兼容旧协议"); break;
    case 2:
        str = tr("华为ECC"); break;
    }
    return str;
}

bool Core_Thread::parameterCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str; bool ret=true, res=true;

    str = tr("设备系列：%1，实际值：%2 ").arg(changeType(desire->devSpec))
                                       .arg(changeType(actual->devSpec));
    if(desire->devSpec == actual->devSpec) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("兼容旧协议：%1，实际值：%2 ").arg(changeProtocol(desire->oldProtocol))
                                        .arg(changeProtocol(actual->oldProtocol));
    if(desire->oldProtocol == actual->oldProtocol) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("语言期望值：%1，实际值：%2 ").arg(desire->language?tr("英文"):tr("中文"))
                                        .arg(actual->language?tr("英文"):tr("中文"));
    if(desire->language == actual->language) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("屏幕期望值：%1，实际值：%2 ").arg(desire->vh?tr("水平"):tr("垂直"))
                                        .arg(actual->vh?tr("水平"):tr("垂直"));
    if(desire->vh == actual->vh) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("版本期望值：%1，实际值：%2 ").arg(changeSpec(desire->standNeutral))
                                        .arg(changeSpec(actual->standNeutral));
    if(desire->standNeutral == actual->standNeutral) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("断路器期望值：%1，实际值：%2 ").arg(desire->isBreaker?tr("存在"):tr("没有"))
                                        .arg(actual->isBreaker?tr("存在"):tr("没有"));
    if(desire->isBreaker == actual->isBreaker) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::devNumCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str ; bool ret=true, res=true;

    str = tr("相数期望值：%1，实际值：%2 ").arg(desire->lineNum ==1?tr("单相"):tr("三相"))
                                        .arg(actual->lineNum ==1?tr("单相"):tr("三相"));
    if(desire->lineNum == actual->lineNum) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("回路数量期望值：%1，实际值：%2 ").arg(desire->loopNum).arg(actual->loopNum);
    if(desire->loopNum == actual->loopNum) ret = true;
    else {res = ret = false;} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::fwCheck()
{
    sVersion *desire = &coreItem.desire.ver;
    sVersion *actual = &coreItem.actual.ver;
    bool ret=true, res=true;

    QString str = tr("软件版本期望值：%1，实际值：%2 ").arg(desire->fwVer).arg(actual->fwVer);
    if(desire->fwVer == actual->fwVer) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    str = tr("设备类型期望值：%1，实际值：%2 ").arg(desire->devType).arg(actual->devType);
    if(desire->devType == actual->devType) ret=true;
    else {res = ret = false;} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::checkErrRange(int exValue, int value, int err)
{
    bool ret = false;
    int min = exValue - err;
    int max = exValue + err;
    if((value>=min) && (value<=max) && value) {
        ret =  true; //qDebug() << "value" << value << exValue << err;
    } else {
        cout << "value Err Range" << value << exValue << err;
    }

    return ret;
}

bool Core_Thread::volErrRange(int i, bool flag)
{
    bool ret = true; double value  = 0; double exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopVol.at(i).toDouble(); exValue = desire->loopVol.at(i).toDouble();}
    else {value = actual->lineVol.at(i).toDouble(); exValue = desire->lineVol.at(i).toDouble();}
    double err = mItem->volErr;

    for(int k=0; k<3; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; else readDev(); //cm_mdelay(100);
    }
    QString str = tr("电压 L%1：期望值=%2V，实测值=%3V ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::curErrRange(int i, bool flag)
{
    bool ret = true; int rated = 100.0;
    double value  = 0; double exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopCur.at(i).toDouble(); exValue = desire->loopCur.at(i).toDouble();}
    else {value = actual->lineCur.at(i).toDouble(); exValue = desire->lineCur.at(i).toDouble();}
    double err = mItem->curErr * 10.0;

    for(int k=0; k<3; ++k) {
        ret = checkErrRange(exValue*rated, value*rated, err);
        if(ret) break; else readDev(); //cm_mdelay(100);
    }
    QString str = tr("电流 L%1：期望值=%2A，实测值=%3A ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::powErrRange(int i, bool flag)
{
    bool ret = true; int rated = RATED;
    double value  = 0; double exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopPow.at(i).toDouble(); exValue = desire->loopPow.at(i).toDouble();}
    else {value = actual->linePow.at(i).toDouble(); exValue = desire->linePow.at(i).toDouble();}
    double err = mItem->powErr * exValue *10.0 ;

    for(int k=0; k<3; ++k) {
        ret = checkErrRange(exValue*rated, value*rated, err);
        if(ret) break; else readDev(); //cm_mdelay(100);
    }
    QString str = tr("功率 L%1：期望值=%2kW，实测值=%3kW ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::checkSquare(double sValue, double pValue, double qValue, double rated)
{
    bool ret = false;
    double errRange = 0.05;
    double appow = sValue* sValue;
    double active = pValue* pValue;
    double reactive = qValue* qValue;
    if(rated == COM_RATE_ELE) errRange = 0.1;

    double sum = active + reactive;
    double err = qSqrt(qAbs(appow - sum));
    if((err < (sValue*errRange))&&(sum)) ret = true;
    else cout << sValue  << pValue << qValue;


//    cout << rated << err << sValue*errRange << pValue << qValue; /////======

    return ret;
}

bool Core_Thread::eleErrRange()
{
    sMonitorData *actual = &coreItem.actual.data;
    bool ret = false; double value = actual->tg_ele;
    QString str = tr("总有功电能：实测值=%1kWh ").arg(value);

    if((value >0)&&(value <10)) {ret = true; str += tr("正常");}
    else {ret = false; str += tr("电能值过大");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::apowErrRange()
{
    sMonitorData *actual = &coreItem.actual.data;
    bool ret = true; int rated = COM_RATE_POW; //RATED /1000;
    double value = actual->apparent_pow;
    double exValue = actual->active_pow;
    double err = actual->reactive_pow;
//    cout << value << exValue << err;

    for(int k=0; k<3; ++k) {
        ret = checkSquare( value, exValue, err, rated);
        if(ret) break; else readDev(); //cm_mdelay(100);
    }

    QString str = tr("总视在功率检查 ");
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::apeleErrRange()
{
    sMonitorData *actual = &coreItem.actual.data;
    bool ret = true; int rated = COM_RATE_ELE; //RATED /100;
    double value = actual->tg_apparentEle;
    double exValue = actual->tg_ele;
    double err = actual->tg_reactiveEle;

    for(int k=0; k<3; ++k) {
        ret = checkSquare( value, exValue , err, rated);
        if(ret) break; else readDev(); //cm_mdelay(100);
    }
    QString str = tr("总视在电能检查 ");
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::errRangeCheck()
{
    int size = 0; bool res = true, ret = true; bool flag = 0;
    sParameter *it = &coreItem.actual.param;
    if(it->loopNum == 0 ) {size = it->lineNum; flag = false;}
    else {size = it->loopNum; flag = true;}

    for(int i = 0; i<size; ++i) {
        ret = volErrRange(i, flag); if(!ret) res = false;
        ret = curErrRange(i, flag); if(!ret) res = false;
        ret = powErrRange(i, flag); if(!ret) res = false;
    }

    ret = eleErrRange(); if(!ret) res = false;
    ret = apowErrRange(); if(!ret) res = false;
//    ret = apeleErrRange(); if(!ret) res = false;

    return res;
}

bool Core_Thread::loopthresholdCheck(int i)
{
    mThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str; bool ret=true, res=true;

    str = tr("回路%1额定电压：期望值%2V，实际值%3V ")
               .arg(i+1).arg(desire->loopVol).arg(actual->loopVol.at(i).toDouble());
    if(desire->loopVol == actual->loopVol.at(i).toDouble()) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str += tr("回路%1额定电流：期望值%2A，实际值%3A ")
               .arg(i+1).arg(desire->loopCur).arg(actual->loopCur.at(i).toDouble());
    if(desire->loopCur == actual->loopCur.at(i).toDouble()) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str += tr("回路%1额定功率：期望值%2kW，实际值%3kW ")
               .arg(i+1).arg(desire->loopPow).arg(actual->loopPow.at(i).toDouble());
    if(desire->loopPow == actual->loopPow.at(i).toDouble()) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::linethresholdCheck(int i)
{
    mThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str; bool ret=true, res=true;

    str = tr("相 L%1额定电压：期望值%2V，实际值%3V ")
               .arg(i+1).arg(desire->lineVol).arg(actual->lineVol.at(i).toDouble());
    if(desire->lineVol == actual->lineVol.at(i).toDouble()) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str = tr("相 L%1额定电流：期望值%2A，实际值%3A ")
               .arg(i+1).arg(desire->lineCur).arg(actual->lineCur.at(i).toDouble());
    if(desire->lineCur == actual->lineCur.at(i).toDouble()){ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str = tr("相 L%1额定功率：期望值%2kW，实际值%3kW ")
               .arg(i+1).arg(desire->linePow).arg(actual->linePow.at(i).toDouble());
    if(desire->linePow == actual->linePow.at(i).toDouble()) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::checkAlarmErr()
{
    bool res = true, ret = true;
    sParameter *it = &coreItem.actual.param;

    if(it->loopNum)
        for(int i=0; i<(int)it->loopNum; ++i) {ret = loopthresholdCheck(i); if(!ret) res = false;}

    if(res)
        for(int i=0; i<(int)it->lineNum; ++i) {ret = linethresholdCheck(i); if(!ret) res = false;}

    return res;
}

bool Core_Thread::alarmCheck()
{
    bool ret = false; if(coreItem.actual.alarm) {
        QString str = tr("设备有报警，请检查：");
        str += "status=" + QString::number(coreItem.actual.alarm);
        emit msgSig(str, ret);
    } else ret = true;
    return ret;
}


bool Core_Thread::envCheck()
{
    bool res = true; bool ret = true;
    sMonitorData *it = &coreItem.actual.data;

    for(int i=0; i<4 && i< it->temps.size(); ++i) {
        double value = it->temps.at(i).toDouble();
        QString str = tr("传感器温度%1：%2°C").arg(i+1).arg(value);
        if(value < 5 || value > 50) {
            ret = res = false; str += tr("错误; ");
            if(i > 1) str += tr("检查传感器盒子连接状态");
        } emit msgSig(str, ret);
    }

    return res;
}

void Core_Thread::initReadCmd(sRtuItem &item)
{
    item.addr = 0x01;
    item.fn = 0x03;
    item.reg = 0x0000;
    item.num = 0x0008;
}

bool Core_Thread::linkCheck()
{
    setModbus();
    bool res = true; sRtuItem itRtu;
    sRtuReplyItem buf;
    initReadCmd(itRtu);
    QString str = tr("级联口检查 ");
    res = mModbus->readCheck(&itRtu, &buf);

    if(res) str += tr("正常"); else { str += tr("错误");}
    emit msgSig(str, res);

    return res;
}

bool Core_Thread::cpuCheck()
{
    bool res = false;
    sParameter *it = &coreItem.actual.param;
    double value = it->cpuTem;

    QString str = tr("CPU温度：%1°C ").arg(value);
    if((value >= 45)&&(value <= 85)) {
        res = true; str += tr("正常");
    } else { str += tr("错误");}
    emit msgSig(str, res);

    return res;
}

bool Core_Thread::readDev(const QString &ip)
{
    http->initHost("192.168.1.31"); readMetaData();
    bool ret = jsonAnalysisRefer(); //if(!ret) emit msgSig(tr("参照设备读取数据失败"), false);
    if(ret){
        http->initHost(m_ips.first()); readMetaData();
        ret = jsonAnalysis(); //if(!ret) emit msgSig(tr("目标设备读取数据失败"), false);
    }

    return ret;
}

bool Core_Thread::workDown(const QString &ip)
{
    bool ret = true;
    emit msgSig(tr("目标设备:")+ip, true);
    ret = readDev(ip);
    if(!ret) emit msgSig("数据读取失败！", ret);
    //---------------------------接口检测--------------------------//
    if(ret && mItem->isSersor) ret = envCheck();
    if(ret && mItem->isLink) ret = linkCheck();

    //--------------------------采集检测---------------------------//
    if(ret) ret = errRangeCheck();
    if(ret) ret = cpuCheck();                           //cpu温度检查
    if(ret) ret = alarmCheck();                         //报警检查

    //--------------------------参数检测--------------------------//
    if(ret) ret = snCheck();
    if(ret) ret = fwCheck();
    if(ret && mItem->isMac) ret = macCheck();
    if(ret && mItem->isTimer) ret = timeCheck();
    if(ret) ret = logoCheck(ip);
    if(ret) ret = parameterCheck();
    if(ret) ret = devNumCheck();
    if(ret) ret = tlsCertCheck(ip);

    //--------------------------阈值检测--------------------------//
    if(ret) ret = checkAlarmErr();

    if(ret) {
        enCascade(0); rtuSet(0);

        emit msgSig("清除所有电能", true); clearAllEle();
        emit msgSig("清除运行时间", true); setRunTime();
        emit msgSig("清除设备日志", true); clearLogs();

        cm_mdelay(1000);
        emit msgSig("恢复出厂设置", true);
        factoryRestore(); cm_mdelay(100);
    }

    return ret;
}

void Core_Thread::run()
{
    bool ret = searchDev(); if(ret) {
        foreach (const auto &ip, m_ips) {
            if(ret) ret = workDown(ip);
            else emit msgSig(tr("目标设备不存在:")+ip, ret);
            emit finshSig(ret, ip+" ");

        } m_ips.clear();
    }  emit overSig();
}
