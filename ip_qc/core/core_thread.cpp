/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"
#include <QImage>
#include <QPixmap>

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    mLogo = "usr/data/pdu/cfg/logo.png";
    QString dir = "usr/data/pdu/cfg";
    FileMgr::build().mkpath(dir);
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
    }
    return ret;
}


bool Core_Thread::timeCheck()
{
    bool ret = false;
    QString str = tr("设备时间，");
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QDateTime dt = QDateTime::fromString(coreItem.datetime, fmd);
    QDateTime time = QDateTime::currentDateTime();
    int secs = qAbs(time.secsTo(dt));
    if(secs > 10) str += tr("相差过大："); else ret = true;
    str += coreItem.datetime;
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::snCheck()
{
    bool ret = false;
    QString sn = coreItem.sn;
    QString str = tr("序列号，");
    if(sn.contains("2I3")) {str += sn; ret = true;}
    else {str += tr("错误：sn=%1").arg(sn);}
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::mcuTempCheck()
{
    bool res = true;
    int cnt = coreItem.actual.param.boardNum;
    QVariantList list = coreItem.mcutemp;
    for(int i=0; i<cnt && i<list.size(); ++i) {
        int temp = list.at(i).toInt(); bool ret = true;
        QString str = tr("第%1块执行板温度%2°c").arg(i+1).arg(temp);
        if(temp > 65 || temp < 5){ res = ret = false; str += tr("错误");}
        emit msgSig(str, ret); //cout << str;
    }

    return res;
}

bool Core_Thread::macCheck()
{
    bool ret = false;
    QString v = coreItem.mac;
    QString str = tr("MAC地址，");
    if(v.contains("2C:26:")) {str += v; ret = true;}
    else {str += tr("错误：mac=%1").arg(v);}
    emit msgSig(str, ret);
    return ret;
}

bool Core_Thread::supplyVolCheck()
{
    sParameter *actual = &coreItem.actual.param;
    bool ret = true; uint vol = actual->supplyVol;
    QString str = tr("12电源检查：vol = %1V").arg(vol / 1000.0);
    if(vol < 11*1000 || vol > 13*1000) {
        str += tr("异常"); ret = false;
    } emit msgSig(str, ret);

    return ret;
}

bool Core_Thread::downLogo(const QString &ip)
{
    QString str = tr("Logo图片下载：");
    QStringList fs; fs << mLogo;

    QFile::remove(mLogo);
    Core_Http *http = Core_Http::bulid(this);
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
    QString str = tr("规格："); bool ret=true, res=true;
    if(desire->devSpec == actual->devSpec) {
        str += tr("%1系列").arg((char)(desire->devSpec+'A'-1));
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                .arg(desire->devSpec).arg(actual->devSpec);
    } emit msgSig(str, ret);

    str = tr("语言："); ret = true;
    if(desire->language == actual->language) {
        if(desire->language) str += tr("英文"); else str += tr("中文");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->language).arg(actual->language);
    } emit msgSig(str, ret);

    str = tr("断路器："); ret = true;
    if(desire->isBreaker == actual->isBreaker) {
        if(desire->isBreaker) str += tr("存在"); else str += tr("没有");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->isBreaker).arg(actual->isBreaker);
    } emit msgSig(str, ret);

    str = tr("屏幕："); ret = true;
    if(desire->vh == actual->vh) {
        if(desire->vh) str += tr("水平"); else str += tr("垂直");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->vh).arg(actual->vh);
    } emit msgSig(str, ret);

    str = tr("中性："); ret = true;
    if(desire->standNeutral == actual->standNeutral) {
        if(desire->standNeutral) str += tr("中性"); else str += tr("标准");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->standNeutral).arg(actual->standNeutral);
    } emit msgSig(str, ret);

//    str = tr("传感器盒子："); ret = true;
//    if(desire->sensorBoxEn == actual->sensorBoxEn) {
//        if(desire->sensorBoxEn) str += tr("存在"); else str += tr("不带");
//    } else {
//        res = ret = false; str += tr("期望值%1,实际值%2")
//                   .arg(desire->sensorBoxEn).arg(actual->sensorBoxEn);
//    } emit msgSig(str, ret);

    return res;
}

bool Core_Thread::devNumCheck()
{
    sParameter *desire = &coreItem.desire.param;
    sParameter *actual = &coreItem.actual.param;
    QString str = tr("设备相数：");
    bool ret=true, res=true;

    if(desire->lineNum == actual->lineNum) {
        if(1==desire->lineNum) str += tr("单相"); else str += tr("三相");
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineNum).arg(actual->lineNum);
    } emit msgSig(str, ret);

    str = tr("执行板数量："); ret = true;
    if(desire->boardNum == actual->boardNum) {
        str += QString::number(desire->boardNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->boardNum).arg(actual->boardNum);
    } emit msgSig(str, ret);

    str = tr("回路数量："); ret = true;
    if(desire->loopNum == actual->loopNum) {
        str += QString::number(desire->loopNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopNum).arg(actual->loopNum);
    } emit msgSig(str, ret);

    str = tr("输出位数量："); ret = true;
    if(desire->outputNum == actual->outputNum) {
        str += QString::number(desire->outputNum);
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->outputNum).arg(actual->outputNum);
    } emit msgSig(str, ret);

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
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->fwVer, actual->fwVer);
    } emit msgSig(str, ret);

    str = tr("设备类型："); ret = true;
        if(desire->devType == actual->devType) {
        str += desire->devType;
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->devType, actual->devType);
    } emit msgSig(str, ret);

    int num = coreItem.actual.param.loopNum;
    for(int i=0; i<num; ++i) {
        str = tr("第%1回路的输出位数量: "); ret = true;
        int a = desire->loopOutlets.at(i).toInt();
        int b = actual->loopOutlets.at(i).toInt();
        if(a == b) {
            str += QString::number(b);
        } else {
            res = ret = false;
            str += tr("期望值%1,实际值%2") .arg(a).arg(b);
        } emit msgSig(str.arg(i+1), ret);
    }

    return res;
}

bool Core_Thread::thresholdCheck()
{
    sThreshold *desire = &coreItem.desire.rate;
    sThreshold *actual = &coreItem.actual.rate;
    QString str = tr("相额定电压：");
    bool ret=true, res=true;
    if(desire->lineVol == actual->lineVol) {
        str += QString::number(desire->lineVol) +"V";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineVol).arg(actual->lineVol);
    } emit msgSig(str, ret);

    str = tr("相额定电流："); ret = true;
    if(desire->lineCur == actual->lineCur) {
        str += QString::number(desire->lineCur) +"A";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->lineCur).arg(actual->lineCur);
    } emit msgSig(str, ret);

    str = tr("相额定功率："); ret = true;
    if(desire->linePow == actual->linePow) {
        str += QString::number(desire->linePow) +"KW";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->linePow).arg(actual->linePow);
    } emit msgSig(str, ret);


    str = tr("回路额定电压："); ret = true;
    if(desire->loopVol == actual->loopVol) {
        str += QString::number(desire->loopVol) +"V";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopVol).arg(actual->loopVol);
    } emit msgSig(str, ret);


    str = tr("回路额定电流："); ret = true;
    if(desire->loopCur == actual->loopCur) {
        str += QString::number(desire->loopCur) +"A";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopCur).arg(actual->loopCur);
    } emit msgSig(str, ret);

    str = tr("回路额定功率："); ret = true;
    if(desire->loopPow == actual->loopPow) {
        str += QString::number(desire->loopPow) +"KW";
    } else {
        res = ret = false; str += tr("期望值%1,实际值%2")
                   .arg(desire->loopPow).arg(actual->loopPow);
    } emit msgSig(str, ret);

    return res;
}


bool Core_Thread::outletCheck()
{
    QVariantList tmpList; bool ret = true; QString str;
    QVariantList *desire = &coreItem.desire.rate.ops;
    QVariantList *actual = &coreItem.actual.rate.ops;
    for(int i=0; i<48; ++i) tmpList << 10;
    for(int i=0; i<desire->size(); ++i) {
        int id = desire->at(i).toInt();
        tmpList[id-1] = 16;
    }

    for(int i=0;i<actual->size(); ++i) {
        double d = tmpList.at(i).toInt();
        double s = actual->at(i).toInt();
        if(s != d) {
            str = tr("输出位%1额定电流：").arg(i+1);
            ret = false; str += tr("期望值%1A,实际值%2A")
                       .arg(d).arg(s);
             emit msgSig(str, ret);
        }
    }

    if(ret) {
        str = tr("输出位额定电流检查：OK");
        emit msgSig(str, ret);
    }

    return ret;
}

bool Core_Thread::outputVolCheck()
{
    bool ret = true;  QString str;
    int spec = coreItem.actual.param.devSpec;
    int num = coreItem.actual.param.outputNum;
    int value = coreItem.actual.rate.volValue * 10.0;
    QVariantList vols = coreItem.actual.rate.outputVols;
    if(spec > 1) {
        for(int i=0; i<vols.size()&&(i<num); ++i) {
            int v = vols.at(i).toInt();
             int s = qAbs(v - value); //cout << v << s;
            if(spec == 3 && !v){ continue; } if(s > 50 || !v) {
                str = tr("输出位%1电压相差过大：相电压：%2V 输出位电压：%3V")
                          .arg(i+1).arg(value/10.0).arg(v/10.0);
                ret = false;  emit msgSig(str, ret);
            }
        }
    }

    if(ret) {
        str = tr("输出位电压检查：OK");
        emit msgSig(str, ret);
    }

    return ret;
}

bool Core_Thread::alarmCheck()
{
    bool ret = false; if(coreItem.alarm) {
        QString str = tr("设备有报警，请检查：");
        str += "status=" + QString::number(coreItem.alarm);
        emit msgSig(str, ret);
    } else ret = true;
    return ret;
}

bool Core_Thread::tgCheck()
{
    bool res = true; bool ret = true;
    sMonitorData *it = &coreItem.actual.data;
    QString str = tr("设备总功率：%1kva").arg(it->apparent_pow);
    if(it->apparent_pow >1) {res = ret = false;  str += tr("过大");} emit msgSig(str, ret);

    str = tr("设备总电能：%1kwh").arg(it->tg_ele); ret = true;
    if(it->tg_ele > 10) {res = ret = false;  str += tr("过大");} emit msgSig(str, ret);

    return res;
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


bool Core_Thread::workDown(const QString &ip)
{
    bool res = true, ret;
    emit msgSig(tr("目标设备:")+ip, true);
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); readMetaData(); jsonAnalysis();
    ret = timeCheck(); if(!ret) res = false;
    ret = snCheck(); if(!ret) res = false;
    ret = fwCheck(); if(!ret) res = false;
    ret = macCheck(); if(!ret) res = false;
    ret = alarmCheck(); if(!ret) res = false;
    ret = devNumCheck(); if(!ret) res = false;
    ret = parameterCheck(); if(!ret) res = false;
    ret = supplyVolCheck(); if(!ret) res = false;
    ret = thresholdCheck(); if(!ret) res = false;
    ret = outputVolCheck(); if(!ret) res = false;
    ret = mcuTempCheck(); if(!ret) res = false;
    ret = outletCheck(); if(!ret) res = false;
    ret = logoCheck(ip); if(!ret) res = false;
    ret = envCheck(); if(!ret) res = false;
    ret = tgCheck(); if(!ret) res = false;

    if(res) {
        enCascade(0); rtuSet(0); //clearAllEle()
        sParameter *desire = &coreItem.desire.param;
        if(0 == desire->sensorBoxEn) boxSet(0);
        if(coreItem.actual.param.devSpec > 2) {
            relayCtrl(1); relayDelay(1); //relayDelay(0);
        } emit msgSig("清除所有电能", true); clearAllEle();
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
            ret = cm_pingNet(ip);
            if(ret) ret = workDown(ip);
            else emit msgSig(tr("目标设备不存在:")+ip, ret);
            emit finshSig(ret, ip+" ");
            sleep(2);
            Json_Pack::bulid()->http_post("testdata/add","192.168.1.12");

        } m_ips.clear();
    }  emit overSig();
}
