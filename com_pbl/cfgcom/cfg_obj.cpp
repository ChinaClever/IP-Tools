/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "cfg_obj.h"

Cfg_Obj::Cfg_Obj(const QString &fn, QObject *parent)
{
    if(!mCfgIni) {
        openCfg(parent, fn);
    }
}

Cfg_Obj::~Cfg_Obj()
{
    if(mCfgIni) delete mCfgIni;
}


/***
  * 获取程序数据目录
  */
QString Cfg_Obj::pathOfCfg(const QString& name)
{    
#if (QT_VERSION > QT_VERSION_CHECK(5,13,0))
    QDir dataDir(QDir::home());  // QCoreApplication::applicationDirPath()
    QString dirName = "." + QCoreApplication::organizationName();
    if(!dataDir.exists(dirName)) {dataDir.mkdir(dirName);} dataDir.cd(dirName);

    dirName = QCoreApplication::applicationName();
    if(!dataDir.exists(dirName)) {dataDir.mkdir(dirName);} dataDir.cd(dirName);
#else
    QDir dataDir("/usr/data/pdu/"); QString dirName = "cfg";
    if(!dataDir.exists(dirName)) {dataDir.mkdir(dirName);} dataDir.cd(dirName);
#endif
    return dataDir.absoluteFilePath(name);
}

/**
 * 功 能：打开系统配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
bool Cfg_Obj::openCfg(QObject *parent, const QString& fn)
{    
    bool ret = QFileInfo::exists(fn);
    if(mCfgIni == nullptr) {
        mCfgIni = new QSettings(fn, QSettings::IniFormat, parent);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
        mCfgIni->setIniCodec(QTextCodec::codecForName("utf-8"));
#endif
    }

    return ret;
}

void Cfg_Obj::writeCfg(const QMap<QString,QVariant> &map, const QString &g)
{
    mCfgIni->beginGroup(g);
    for(auto &it: map.toStdMap()) {
        mCfgIni->setValue(it.first, it.second);
    }
    mCfgIni->endGroup();
    //mCfgIni->sync();
}

/**
 * 功 能：参数写入配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
void Cfg_Obj::writeCfg(const QString& key, const QVariant &v, const QString& g)
{
    QMap<QString,QVariant> map;
    map.insert(key, v); writeCfg(map, g);
}

QVariantList Cfg_Obj::readCfg(const QStringList &keys, const QString &g)
{
    QVariantList res; mCfgIni->beginGroup(g);
    foreach(auto &it, keys) res << mCfgIni->value(it,"");
    mCfgIni->endGroup(); return res;
}

QMap<QString,QVariant> Cfg_Obj::readCfg(const QMap<QString,QVariant> &keys, const QString &g)
{
    QMap<QString,QVariant> res; mCfgIni->beginGroup(g);
    for(auto &it: keys.toStdMap()){
        res.insert(it.first, mCfgIni->value(it.first, it.second));
    } mCfgIni->endGroup(); return res;
}

QVariant Cfg_Obj::readCfg(const QString &key, const QVariant &v, const QString& g)
{
    mCfgIni->beginGroup(g);
    QVariant ret = mCfgIni->value(key, v);
    mCfgIni->endGroup();

    return ret;
}
