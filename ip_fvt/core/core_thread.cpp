/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "core_thread.h"

Core_Thread::Core_Thread(QObject *parent)
    : Core_Object{parent}
{
    Ssdp_Core::bulid(this);
    Core_Http::bulid(this);
    mPro = sDataPacket::bulid()->getPro();
    connect(this, &Core_Thread::startSig, &Core_Thread::startSlot);
}

QStringList Core_Thread::getFs()
{
    FileMgr::build().mkpath("usr/data/pdu/doc/");
    QString dir = "usr/data/pdu/cfg/"; FileMgr::build().mkpath(dir);
    QStringList fs; fs << "usr/data/pdu/ver.ini" << "usr/data/pdu/doc/modbus.xlsx";
    fs << dir+"alarm.conf" << dir+"devParam.ini" << dir+"cfg.ini" << dir+"inet.ini";
    fs << dir+"alarm.cfg" << dir+"snmpd.conf" << dir+"logo.png" << dir+"mac.conf";
    fs << dir + "sn.conf";
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

void Core_Thread::timeSync()
{
    QString fmd = "yyyy-MM-dd hh:mm:ss";
    QString t = QDateTime::currentDateTime().toString(fmd);
    emit msgSig(tr("时间设置：")+t, true);
    Core_Http *http = Core_Http::bulid(this);
    sCfgItem it; it.type = 43; it.fc =1;
    http->setting(it, t); cm_mdelay(321);
    it.type = 30; it.fc = 9;
    http->setting(it, t);
}


void Core_Thread::enModbusRtu()
{
    sCfgItem it; it.type = 15; it.fc = 1;
    Core_Http::bulid(this)->setting(it, 1);
    it.fc = 7; Core_Http::bulid(this)->setting(it, 1);
    emit msgSig(tr("设备模式：已开启Modbus-RTU功能"), true);

    //it.type = 13; it.fc = 3;
    //Core_Http::bulid(this)->setting(it, 1);
    //emit msgSig(tr("设备模式：已开启设备级联功能"), true);

    it.type = 13; it.fc = 9;
    Core_Http::bulid(this)->setting(it, 1);
    emit msgSig(tr("启用扩展口：已开启传感器盒子功能"), true);
}

void Core_Thread::writeSnMac(const QString &sn, const QString &mac)
{
    QString dir = "usr/data/pdu/cfg/"; QFile file(dir + "mac.conf");
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
    QStringList fs; fs << "usr/data/pdu/ver.ini";
    QFile::remove(fs.first());
    Core_Http *http = Core_Http::bulid(this);
    http->initHost(ip); http->downFile(fs);
    for(int i=0; i<1000; i+= 100) {
        if(QFile::exists(fs.first())) break; else cm_mdelay(100);
    } QString dir = "usr/data/pdu";
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
    Core_Http *http = Core_Http::bulid(this);
    QStringList fs = getFs(); bool res = true;
    foreach (const auto fn, fs) {
        bool ret = http->uploadFile(fn);
        if(!ret) res = false;
        emit msgSig(fn, ret);
        cm_mdelay(20);
    }

    if(res) {
        emit msgSig(tr("设备重启，设备有响声"), true);        
        http->execute("sync"); cm_mdelay(1000);
        http->execute("reboot"); // killall cores
    }
    return res;
}

void Core_Thread::run()
{
    bool ret = searchDev();
    if(ret && fsCheck()) {
        foreach (const auto &ip, m_ips) {            
            emit msgSig(tr("目标设备:")+ip, true);
            ret = downVer(ip); timeSync();
            if(ret) ret = workDown(ip);
            if(ret) enModbusRtu(); cm_mdelay(150);
            emit finshSig(ret, ip+" ");
#if 0
            cm_mdelay(2000);
            Json_Pack::bulid()->http_post("debugdata/add","192.168.1.12");
#endif
        }m_ips.clear();
    } emit overSig();
}
