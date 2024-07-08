#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "core_thread.h"
#include "datapacket.h"
#include "json_pack.h"

namespace Ui {
class Home_WorkWid;
}

struct sCount
{
    sCount() {all=ok=err=0; cnt=100;}
    int cnt;
    int all;
    int ok;
    int err;
};

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();

private:
    bool initMac();
    bool initWid();
    bool initUser();
    bool inputCheck();
    void initData();
    void logWrite();

    QString getTime();
    void initLayout();
    void updateTime();
    bool updateWid();
    bool initHost();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);
    void writeSnMac(const QString &sn, const QString &mac);

private slots:
    void timeoutDone();
    void initFunSlot();
    void updateCntSlot();
    void updateResult();
    void on_startBtn_clicked();
    void on_findBtn_clicked();
    void on_downBtn_clicked();
    void finishSlot(bool pass, const QString &msg);
    void insertTextSlot(const QString &msg, bool pass);
    void on_adCheckBox_clicked(bool checked);

    void on_userEdit_selectionChanged();

private:
    Ui::Home_WorkWid *ui;
    Core_Thread *mCoreThread;
    QTimer *timer=nullptr;
    bool isStart=false;
    bool mResult=true;
    QTime startTime;
    sCount mCnt;
    int mId=1;
    sDataPacket *mPro;
};

#endif // HOME_WORKWID_H
