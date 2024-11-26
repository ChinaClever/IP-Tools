#include "yc_obj.h"

Yc_Obj::Yc_Obj(QObject *parent) : BaseThread(parent)
{
    mAc = YC_Ac92b::bulid(this);
    mDc = YC_Dc107::bulid(this);
}


Yc_Obj *Yc_Obj::bulid(QObject *parent)
{
    static Yc_Obj* sington = nullptr;
    if(sington == nullptr)
        sington = new Yc_Obj(parent);
    return sington;
}

YC_StandSource *Yc_Obj::get()
{
    YC_StandSource *yc = mAc;


    return yc;
}

bool Yc_Obj::powerOn(int v)
{
    YC_StandSource *yc = get(); yc->setVol(220);
    QString str = tr("连接标准源");
    bool ret = yc->handShake();
    if(!ret) {
        str = tr("再次连接标准源");
        ret = yc->handShake();
    }
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret);

    str = tr("标准源上电");
    if(ret) ret = yc->powerOn(v);
    if(ret) str += tr("成功"); else str += tr("失败");
    emit msgSig(str, ret);

    return ret;
}

bool Yc_Obj::powerDown()
{
    YC_StandSource *yc = get();
    return yc->powerDown();
}

bool Yc_Obj::setCur(int v, int sec)
{
    YC_StandSource *yc = get();
    return yc->setCur(v, sec);
}

bool Yc_Obj::setVol(int v, int sec)
{
    YC_StandSource *yc = get();
    return yc->setVol(v, sec);
}
