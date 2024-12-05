/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#include <QImage>
#include <QPixmap>
#include "dbmacs.h"

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    mLogo = "usr/data/pdu/cfg/logo.png";
    QString dir = "usr/data/pdu/cfg";
    FileMgr::build().mkpath(dir);
    mItem = CfgCom::bulid()->item;
    http = Core_Http::bulid(this);
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

bool Core_Thread::parameterCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str; bool ret=true, res=true;

    str = tr("语言期望值：%1，实际值：%2 ").arg(desire->language?tr("英文"):tr("中文"))
                                        .arg(actual->language?tr("英文"):tr("中文"));
    if(desire->language == actual->language) ret=true;
    else {res = ret = false;}emit msgSig(str, ret);

    str = tr("屏幕期望值：%1，实际值：%2 ").arg(desire->vh?tr("水平"):tr("垂直"))
                                        .arg(actual->vh?tr("水平"):tr("垂直"));
    if(desire->vh == actual->vh) ret=true;
    else {res = ret = false;}emit msgSig(str, ret);

    str = tr("中性期望值：%1，实际值：%2 ").arg(desire->standNeutral?tr("水平"):tr("垂直"))
                                        .arg(actual->standNeutral?tr("水平"):tr("垂直"));

    if(desire->standNeutral == actual->standNeutral) ret=true;
    else {res = ret = false;}emit msgSig(str, ret);

    return res;
}

bool Core_Thread::devNumCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str ; bool ret=true, res=true;

    str = tr("相数：期望值%1，实际值%2 ").arg(desire->lineNum ==1?tr("单相"):tr("三相"))
                                        .arg(actual->lineNum ==1?tr("单相"):tr("三相"));
    if(desire->lineNum == actual->lineNum) res=true;
    else {res = ret = false;  emit msgSig(str, ret);}

    str = tr("回路数量：期望值%1，实际值%2 ").arg(desire->loopNum).arg(actual->loopNum);
    if(desire->loopNum == actual->loopNum) ret = true;
    else {res = ret = false;  emit msgSig(str, ret);}

    return res;
}

bool Core_Thread::fwCheck()
{
    sVersion *desire = &coreItem.desire.ver;
    sVersion *actual = &coreItem.actual.ver;
    bool ret=true, res=true;

    QString str = tr("软件版本号："); ret = true;
    if(desire->fwVer == actual->fwVer) {
        str += desire->fwVer;
    } else {
        res = ret = false; str += tr("期望值%1，实际值%2")
                   .arg(desire->fwVer, actual->fwVer);
    } emit msgSig(str, ret);

    str = tr("设备类型："); ret = true;
        if(desire->devType == actual->devType) {
        str += desire->devType;
    } else {
        res = ret = false; str += tr("期望值%1，实际值%2")
                   .arg(desire->devType, actual->devType);
    } emit msgSig(str, ret);

    return res;
}

bool Core_Thread::checkErrRange(int exValue, int value, int err)
{
    bool ret = false;
    int min = exValue - err;
    int max = exValue + err;
    if((value>=min) && (value<=max) && value) {
        ret =  true;
    } else {
        qDebug() << "value Err Range" << value << exValue << err;
    }

    return ret;
}

bool Core_Thread::volErrRange(int i, bool flag)
{
    bool ret = true; int value  = 0; int exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopVol.at(i).toDouble(); exValue = desire->loopVol.at(i).toDouble();}
    else {value = actual->lineVol.at(i).toDouble(); exValue = desire->lineVol.at(i).toDouble();}
    int err = mItem->volErr;

    for(int k=0; k<5; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; {cm_mdelay(200); readDev();}
    }

    QString str = tr("电压 L%1：期望值=%2V，实测值=%3V ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::curErrRange(int i, bool flag)
{
    bool ret = true; int rated = 10.0;
    int value  = 0; int exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopCur.at(i).toDouble(); exValue = desire->loopCur.at(i).toDouble();}
    else {value = actual->lineCur.at(i).toDouble(); exValue = desire->lineCur.at(i).toDouble();}
    int err = mItem->curErr / rated;

    for(int k=0; k<5; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; {cm_mdelay(200); readDev();}
    }

    QString str = tr("电流 L%1：期望值=%2A，实测值=%3A ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::powErrRange(int i, bool flag)
{
    bool ret = true; int rated = 10.0;
    int value  = 0; int exValue = 0;
    sPdudata *actual = &coreItem.actual.value;
    sPdudata *desire = &coreItem.desire.value;
    if(flag) {value = actual->loopPow.at(i).toDouble(); exValue = desire->loopPow.at(i).toDouble();}
    else {value = actual->linePow.at(i).toDouble(); exValue = desire->linePow.at(i).toDouble();}
    int err = mItem->powErr / rated;

    for(int k=0; k<5; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; {cm_mdelay(200); readDev();}
    }

    QString str = tr("功率 L%1：期望值=%2kW，实测值=%3kW ").arg(i+1).arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::eleErrRange()
{
    bool ret = true; int rated = 10.0;
    sMonitorData *actual = &coreItem.actual.data;
    sMonitorData *desire = &coreItem.desire.data;
    int value = actual->tg_ele;
    int exValue = desire->tg_ele;
    int err = mItem->powErr / rated;

    for(int k=0; k<5; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; else {cm_mdelay(200); readDev();}
    }

    QString str = tr("总电能：期望值=%2kWh，实测值=%3kWh ").arg(exValue).arg(value);
    if(ret) str += tr("正常"); else {str += tr("错误");}

    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::apowErrRange()
{
    bool ret = true; int rated = 10.0;
    sMonitorData *actual = &coreItem.actual.data;
    sMonitorData *desire = &coreItem.desire.data;
    double value = actual->apparent_pow;
    double exValue = desire->apparent_pow;
    int err = mItem->powErr / rated;

    for(int k=0; k<5; ++k) {
        ret = checkErrRange(exValue, value, err);
        if(ret) break; else {cm_mdelay(200); readDev();}
    }

    QString str = tr("总视在功率：期望值=%2kVA，实测值=%3kVA ").arg(exValue).arg(value);
    if(ret) str += tr("正常");
    else {str += tr("错误");}

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

    return res;
}

bool Core_Thread::loopthresholdCheck(int i)
{
    sThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str;
    bool ret=true, res=true;

    str = tr("回路%1额定电压：期望值%2V，实际值%3V ")
               .arg(i+1).arg(desire->loopVol.at(i).toDouble()).arg(actual->loopVol.at(i).toDouble());
    if(desire->loopVol == actual->loopVol) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str += tr("回路%1额定电流：期望值%2A，实际值%3A ")
               .arg(i+1).arg(desire->loopCur.at(i).toDouble()).arg(actual->loopCur.at(i).toDouble());
    if(desire->loopCur == actual->loopCur) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str += tr("回路%1额定功率：期望值%2kW，实际值%3kW ")
               .arg(i+1).arg(desire->loopPow.at(i).toDouble()).arg(actual->loopPow.at(i).toDouble());
    if(desire->loopPow == actual->loopPow) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    return res;
}

bool Core_Thread::linethresholdCheck(int i)
{
    sThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str;
    bool ret=true, res=true;

    str = tr("相 L%1额定电压：期望值%2V，实际值%3V ")
               .arg(i+1).arg(desire->lineVol.at(i).toDouble()).arg(actual->lineVol.at(i).toDouble());
    if(desire->lineVol == actual->lineVol) {ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str = tr("相 L%1额定电流：期望值%2A，实际值%3A ")
               .arg(i+1).arg(desire->lineCur.at(i).toDouble()).arg(actual->lineCur.at(i).toDouble());
    if(desire->lineCur == actual->lineCur){ret = true; str += tr("正常");}
    else {res = ret = false; str += tr("错误");} emit msgSig(str, ret);

    str = tr("相 L%1额定功率：期望值%2kW，实际值%3kW ")
               .arg(i+1).arg(desire->linePow.at(i).toDouble()).arg(actual->linePow.at(i).toDouble());
    if(desire->linePow == actual->linePow) {ret = true; str += tr("正常");}
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
        }
        emit msgSig(str, ret);
    }

    for(int i=0; i<2 && i< it->doors.size(); ++i) {
        int value = it->doors.at(i).toInt();
        QString str = tr("门禁%1：%2").arg(i+1).arg(value);
        if(0 == value) {ret = res = false; str += tr("错误");}
        emit msgSig(str, ret);
    }

     return res;
}

bool Core_Thread::cpuCheck()
{
    bool res = true;
    sParameter *it = &coreItem.actual.param;
    double value = it->cpuTem;

    QString str = tr("CPU温度：%1°C ").arg(value);
    if((value < 5)&&(value > 90)) {
        res = false; str += tr("错误");
    }
    emit msgSig(str, res);

    return res;
}

void Core_Thread::readDev(const QString &ip)
{
    http->initHost(ip); readMetaData(); jsonAnalysis();
    http->initHost("192.168.1.33"); readMetaData(); jsonAnalysisRefer();
}

bool Core_Thread::workDown(const QString &ip)
{
    bool res = true, ret;
    emit msgSig(tr("目标设备:")+ip, true);
    readDev(ip);
    //---------------------------接口检测--------------------------//

    // ret = envCheck(); if(!ret) res = false;

    //--------------------------采集检测---------------------------//
    ret = errRangeCheck(); if(!ret) res = false;
    ret = cpuCheck(); if(!ret) res = false;        //cpu温度检查
    ret = alarmCheck(); if(!ret) res = false;     //报警检查

    //--------------------------参数检测--------------------------//
    ret = snCheck(); if(!ret) res = false;
    ret = macCheck(); if(!ret) res = false;
    ret = fwCheck(); if(!ret) res = false;
    ret = timeCheck(); if(!ret) res = false;
    ret = logoCheck(ip); if(!ret) res = false;
    ret = parameterCheck(); if(!ret) res = false;
    ret = devNumCheck(); if(!ret) res = false;

    //--------------------------阈值检测--------------------------//
    ret = checkAlarmErr(); if(!ret) res = false;

    if(res) {
        enCascade(0); rtuSet(0); //clearAllEle();
        // sParameter *desire = &coreItem.desire.param;=======
        // if(0 == desire->sensorBoxEn) boxSet(0);============
        // if(coreItem.actual.param.devSpec > 2) {============
        //     relayCtrl(1); relayDelay(1); //relayDelay(0);==
        // }

        emit msgSig("清除所有电能", true); clearAllEle();
        emit msgSig("清除运行时间", true); setRunTime();
        emit msgSig("清除设备日志", true); clearLogs();

        cm_mdelay(1000);
        emit msgSig("恢复出厂设置", true);
        factoryRestore(); cm_mdelay(100);
    }

    return res;
}

void Core_Thread::run()
{
    bool ret = searchDev(); if(ret) {
        foreach (const auto &ip, m_ips) {
            // ret = cm_pingNet(ip);
            if(ret) ret = workDown(ip);
            else emit msgSig(tr("目标设备不存在:")+ip, ret);
            emit finshSig(ret, ip+" ");

        } m_ips.clear();
    }  emit overSig();
}
