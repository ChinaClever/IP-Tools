#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H
#include "cfg_file.h"
#include "home_statewid.h"
#include "core_sender.h"

namespace Ui {
class Home_WorkWid;
}

struct sCount
{
    int allCnt;
    int dstCnt;
    int slaveCnt;
    int okCnt;
    int errCnt;
};

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();

protected:
    void initLayout();
    QString getTime();
    void updateTime();
    void setWidEnabled(bool en);
    void setTextColor(bool pass);

    bool initWid();
    bool inputCheck();
    void initData(sOtaFile &it);
    bool fileCrc(const QString &fn);
    void initCnt(int dst, int slave);
    bool readRsaSig(const QString &name);
    QStringList getIpList();

private slots:
    void timeoutDone();
    void initFunSlot();    
    void updateResult();

    void updateCntSlot();
    void on_imgBtn_clicked();
    void on_startBtn_clicked();
    void on_searchBtn_clicked();
    void updateWidSlot(){initData(mFileIt);}
    void finishSlot(bool pass, const QString &msg);
    void on_searchBox_currentIndexChanged(int index);
    void insertTextSlot(bool pass, const QString &msg);
    void upgradeError(const QString &host, const QString &msg);

private:
    Ui::Home_WorkWid *ui;

    int mId;
    sCount mCnt;
    bool isStart;
    bool mResult;
    QTimer *timer;
    QTime startTime;
    Ssdp_Core *mSsdp;
    sOtaFile mFileIt;
    Core_Sender *mSender;

};

#endif // HOME_WORKWID_H
