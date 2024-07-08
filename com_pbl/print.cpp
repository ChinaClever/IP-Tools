/*
 * common.cpp
 * 公共方法接口
 *
 *  Created on: 2018年1月1日
 *      Author: Lzy
 */
#include "print.h"
#include <QWidget>
#include <QHeaderView>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


void cm_mdelay(int msec)
{
    if (msec <= 0) {
        return;
    }

#if 1
    //非阻塞方式延时,现在很多人推荐的方法
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    //阻塞方式延时,如果在主线程会卡住主界面
    QThread::msleep(msec);
#else
    //非阻塞方式延时,不会卡住主界面,据说可能有问题
    QTime endTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < endTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
#endif
}

QString cm_execute(const QString &cmd)
{
    QProcess pro;
    pro.start(cmd);
    pro.waitForFinished();
    QByteArray bs = pro.readAllStandardOutput();
    bs +=  pro.readAllStandardError();
    return QString::fromLocal8Bit(bs);
}


bool cm_pingNet(const QString& ip)
{
    QProcess pingProcess;
#if (QT_VERSION > QT_VERSION_CHECK(6,0,0))
    QStringList strArg; strArg << ip << " -n 1 -i 2";  //strPingIP 为设备IP地址
    pingProcess.start("ping", strArg, QIODevice::ReadOnly);
#else
    QString strArg = "ping " + ip + " -n 1 -i 2";  //strPingIP 为设备IP地址
    pingProcess.start(strArg, QIODevice::ReadOnly);
#endif
    pingProcess.waitForFinished(-1);
    QString p_stdout = QString::fromLocal8Bit(pingProcess.readAllStandardOutput());
    bool bPingSuccess = false; //

    if(p_stdout.contains("TTL=")) { //我采用这个字符来判断 对不对？
        bPingSuccess = true;
    }else {
        bPingSuccess = false;
        qDebug() << p_stdout;
    } pingProcess.kill();

    return bPingSuccess;
}

/***
  *判断一个字符串是否为纯数字
  */
bool cm_isDigitStr(const QString &src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();
    while(*s && *s>='0' && *s<='9') s++;
    if (*s) return false; //不是纯数字
    return true;
}

/**
 * @brief 是否为IP地址
 * @param ip
 * @return
 */
bool cm_isIPaddress(const QString& ip)
{
#if (QT_VERSION > QT_VERSION_CHECK(6,0,0))
    QString exp = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
    QRegularExpression v(exp);
    QRegularExpressionMatch match = v.match(ip);
    if(match.hasMatch())  return true;
#else
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(regExp.exactMatch(ip)) return true ;
#endif
    return false;
}


/**
 * @brief 转16进制
 * @param array
 * @return
 */
QString cm_ByteArrayToHexStr(const QByteArray &array)
{
    QString strArray; // 十六进制
    for(int i=0; i<array.size(); ++i) {
        strArray.append(QString("%1 ").arg((uchar)array.at(i), 2, 16, (QChar)'0'));
    }
    return strArray.toUpper().simplified();
}

/**
 * @brief 转十进制
 * @param array
 * @return
 */
QString cm_ByteArrayToUcharStr(const QByteArray &array)
{
    QString strArray ;
    for(int i=0; i<array.size(); ++i)
        strArray += QString::number((uchar)array.at(i)) + " ";  // 十进制
    return strArray;
}


QString cm_ByteArrayToHexStr(uchar *buf, int len)
{
    QByteArray array((char *)buf, len);
    return cm_ByteArrayToHexStr(array);
}
