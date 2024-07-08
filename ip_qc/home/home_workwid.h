#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "home_devwid.h"
#include "logmainwid.h"
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
    void devSetBtn();

signals:
    void startSig();

private:
    bool initWid();
    bool initUser();
    bool inputCheck();
    void initData();
    void logWrite();

    QString getTime();
    void initLayout();
    void updateTime();
    bool updateWid();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);

private slots:
    void updateWidSlot();
    void timeoutDone();
    void initFunSlot();
    void updateCntSlot();
    void updateResult();
    void on_startBtn_clicked();
    void on_findBtn_clicked();
    void on_logoBtn_clicked();
    void on_auCheckBox_clicked(bool checked);
    void finishSlot(bool pass, const QString &msg);
    void insertTextSlot(const QString &msg, bool pass);

    void on_ipEdit_textChanged(const QString &arg1);

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
