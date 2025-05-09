/*
 *
 *
 *  Created on: 2020年10月11日
 *      Author: Lzy
 */
#include "dblogs.h"

DbLogs::DbLogs()
{
    createTable();
    tableTile = tr("日志");
    //hiddens <<  9;
    headList << tr("客户名称") << tr("设备类型") << tr("软件版本")
             << tr("物理地址") << tr("结果") << tr("序列号");
}

void DbLogs::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "user           VCHAR,"
            "dev            VCHAR,"
            "sw             VCHAR,"
            "mac            VCHAR,"
            "result         VCHAR,"
            "sn             VCHAR not null);";
    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName()))) {
        throwError(query.lastError());
    }
}


DbLogs *DbLogs::bulid()
{
    static DbLogs* sington = nullptr;
    if(sington == nullptr)
        sington = new DbLogs();
    return sington;
}

bool DbLogs::insertItem(const sLogItem &item)
{
    QString cmd = "insert into %1 (date,time,user,dev,sw,mac,result,sn) "
                  "values(:date,:time,:user,:dev,:sw,:mac,:result,:sn)";
    bool ret = modifyItem(item,cmd.arg(tableName()));
    if(ret) emit itemChanged(item.id, Insert);
    return ret;
}

bool DbLogs::modifyItem(const sLogItem &item, const QString &cmd)
{
    QSqlQuery query(mDb);
    query.prepare(cmd);

    query.bindValue(":date",item.date);
    query.bindValue(":time",item.time);
    query.bindValue(":user",item.user);
    query.bindValue(":dev",item.dev);
    query.bindValue(":sw",item.sw);
    query.bindValue(":mac",item.mac);
    query.bindValue(":result",item.result);
    query.bindValue(":sn",item.sn);
    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}


int DbLogs::contains(const QString &mac, const QString &sn)
{
    QString condition = QString("where mac=\'%1\' AND sn!='%2'").arg(mac, sn);

    return count("id", condition);
}
