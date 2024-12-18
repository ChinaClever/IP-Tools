#include "baseobject.h"
#include "baselogs.h"

BaseObject::BaseObject()
{
    // mModbus = nullptr;
    mPacket = sDataPacket::bulid();
    mItem = CfgCom::bulid()->item;
    mPro = mPacket->getPro();
    mDev = mPacket->getDev();
    mData = &(mDev->data);
    // mCfg = &(mDev->cfg);
    // mDt = &(mDev->dt);
}

BaseThread::BaseThread(QObject *parent) : QThread(parent)
{
    isRun = false;
    BaseLogs::bulid(this);
    int t = 550+rand()%150;
    QTimer::singleShot(t,this,SLOT(initFunSlot()));
}

BaseThread::~BaseThread()
{
    isRun = false;
    wait();
}

void BaseThread::initFunSlot()
{
    // mModbus = Rtu_Modbus::bulid(this)->get();
    // if(!mModbus) {
    //     int t = 100+rand()%50;
    //     QTimer::singleShot(t,this,SLOT(initFunSlot()));
    // }
}

bool BaseThread::updatePro(const QString &str, bool pass, int sec)
{
    mPro->time = QTime::currentTime().toString("hh:mm:ss");
    mPro->pass << pass;
    mPro->status << str;
    if(pass) pass = delay(sec);
    else mPro->result = Test_Fail;
    if(!pass) BaseLogs::bulid()->appendLogItem(str, pass);

    return pass;
}

bool BaseThread::updateValuePro(const QString &str, bool pass, int sec)
{
    mPro->time = QTime::currentTime().toString("hh:mm:ss");
    mPro->pass << pass;
    mPro->status << str;
    if(pass) pass = delay(sec);
    else mPro->result = Test_Fail;
    if(pass) BaseLogs::bulid()->appendValueItem(str, pass);

    return pass;
}

bool BaseThread::mdelay(int s)
{
    bool ret = true;
    for(int i=0; i<s; ++i) {
        if(mPro->step < Test_Over){
            QThread::msleep(100);
        } else {
            ret = false;
            break;
        }
    }

    return ret;
}
