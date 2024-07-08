/*
 * 配置文件公共基类
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "cfgserial.h"

CfgSerial::CfgSerial(const QString &fn, QObject *parent) : Cfg_Obj(fn, parent)
{

}

/**
 * @brief 获取串口名称
 * @return 串口名
 */
QString CfgSerial::getSerialName(const QString &key)
{
    return readCfg(key, "", "Serial").toString();
}

/**
 * @brief 设置串口名
 * @param name
 */
void CfgSerial::setSerialName(const QString &key, const QString &v)
{
    writeCfg(key, v, "Serial");
}

QString CfgSerial::getSerialBr(const QString &com)
{
    QString key = QString("BR_%1").arg(com);
    return readCfg(key, "", "Serial").toString();
}

void CfgSerial::setSerialBr(const QString &com, const QString &br)
{
    QString key = QString("BR_%1").arg(com);
    writeCfg(key, br, "Serial");
}

/**
 * @brief 获取当前用户名称
 * @return 用户名
 */
QString CfgSerial::getLoginName()
{
    return readCfg("name", "admin", "Login").toString();
}

/**
 * @brief 设置当前用户名
 * @param name
 */
void CfgSerial::setLoginName(const QString &name)
{
    writeCfg("name", name, "Login");
}
