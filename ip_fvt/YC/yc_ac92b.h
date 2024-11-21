#ifndef YC_AC92B_H
#define YC_AC92B_H

#include "yc_standsource.h"

class YC_Ac92b : public YC_StandSource
{
    Q_OBJECT
    explicit YC_Ac92b(QObject *parent = nullptr);
public:
    static YC_Ac92b *bulid(QObject *parent = nullptr);

    bool handShake();
    bool setCur(int v, int sec=0);
    bool setVol(int v, int sec=0);

protected:
    bool initFun();
    bool setRange();
    bool setValue(const QString &str, int v);
    bool serialWrite(const QByteArray &array);
};

#endif // YC_AC92B_H
