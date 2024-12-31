#ifndef TEST_YC_H
#define TEST_YC_H

#include "yc_dc107.h"

class Yc_Obj : public BaseThread
{
    Q_OBJECT
    explicit Yc_Obj(QObject *parent = nullptr);
public:
    static Yc_Obj *bulid(QObject *parent = nullptr);
    YC_StandSource *get();

    bool powerOn(int v=60);
    bool powerDown();

    bool setCur(int v, int sec=0);
    bool setVol(int v, int sec=0);

signals:
    void msgSig(const QString &msg, bool pass);

private:
    YC_Ac92b *mAc;
};

#endif // TEST_YC_H
