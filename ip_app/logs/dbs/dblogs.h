#ifndef DBLOGS_H
#define DBLOGS_H
#include "basicsql.h"

struct sLogItem : public DbBasicItem{
    QString dev, fn, md5, user, sw, old, remark;
};

class DbLogs : public SqlBasic<sLogItem>
{
    DbLogs();
public:
    static DbLogs* bulid();
    QString tableName(){return "packs";}
    bool insertItem(const sLogItem& item);

protected:
    void createTable();
    bool modifyItem(const sLogItem& item,const QString& cmd);
};

#endif // DBLOGS_H
