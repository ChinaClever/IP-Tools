#ifndef CFG_OBJ_H
#define CFG_OBJ_H
#include <QtCore>

#define CFG_FN "cfg.ini"

class Cfg_Obj
{
public:
    Cfg_Obj(const QString& fn, QObject *parent = nullptr);
    static QString pathOfCfg(const QString& name);
    ~Cfg_Obj();

    void writeCfg(const QMap<QString,QVariant> &map, const QString &g="");
    void writeCfg(const QString &key, const QVariant& v, const QString &g="");

    QVariantList readCfg(const QStringList &keys, const QString &g="");
    QMap<QString,QVariant> readCfg(const QMap<QString, QVariant> &keys, const QString &g="");
    QVariant readCfg(const QString &key, const QVariant &v = QVariant(), const QString &g="");

private:
    bool openCfg(QObject *parent = nullptr, const QString& fn = CFG_FN);

private:
    QSettings *mCfgIni=nullptr;
};

#endif // CFG_OBJ_H
