#ifndef CFG_FILE_H
#define CFG_FILE_H
#include "cfg_obj.h"

struct sOtaFile {
    sOtaFile():fc(1), path("/usr/data/upload/"), size(0){}
    ushort fc;
    QString dev;
    QString path;
    QString file;
    QString md5;
    QString sig;
    uint size;
    ushort crc;
};

namespace File {
    QString md5(const QString &fn);
    QStringList entryList(const QString &fn);
    uint fileSize(const QString &fn);
    bool CheckMd5(const sOtaFile &it);
    bool CheckMd5(const QString &fn);
    bool AppendMd5(const QString &fn);
    QString certFile();
    QString keyFile();
};

#define START_HEAD ((ushort)0xC5C5)
#define END_CRC ((ushort)0x5C5C)

#endif // CFG_FILE_H
