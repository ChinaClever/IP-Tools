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
    tableTile = tr("打包日志");
    //hiddens <<  9;
    headList << tr("客户名称") << tr("设备类型") << tr("程序名称") << tr("发布版本") << tr("依赖版本") << tr("MD5校验码") << tr("发布说明");
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
            "fn             VCHAR,"
            "sw             VCHAR,"
            "old            VCHAR,"
            "md5            VCHAR not null,"
            "remark         VCHAR);";
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
    QString cmd = "insert into %1 (date,time,fn,md5,user,dev,sw,old,remark) "
                  "values(:date,:time,:fn,:md5,:user,:dev,:sw,:old,:remark)";
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
    query.bindValue(":fn",item.fn);
    query.bindValue(":sw",item.sw);
    query.bindValue(":old",item.old);
    query.bindValue(":md5",item.md5);
    query.bindValue(":remark",item.remark);
    bool ret = query.exec();
    if(!ret) throwError(query.lastError());
    return ret;
}
