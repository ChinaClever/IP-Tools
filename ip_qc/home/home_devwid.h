#ifndef HOME_DEVWID_H
#define HOME_DEVWID_H

#include <QWidget>
#include <QCheckBox>
#include "backcolourcom.h"
#include "core_thread.h"

namespace Ui {
class Home_DevWid;
}

class Home_DevWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_DevWid(QWidget *parent = nullptr);
    ~Home_DevWid();

public slots:
    void updateData();

private:
    void updateParams();
    void updateThresholds();
    void initWid();
    void updateWid();

private slots:
    void on_eleClearBtn_clicked();
    void on_factoryBtn_clicked();
    void on_lineVolBox_valueChanged(int arg1);
    void on_lineCurBox_valueChanged(int arg1);
    void on_loopVolBox_valueChanged(int arg1);
    void on_loopCurBox_valueChanged(int arg1);

    void on_logBtn_clicked();
    void on_cascadeBtn_clicked();
    void on_rtuEnBtn_clicked();
    void on_rtuDisenBtn_clicked();
    void on_loopNumBox_valueChanged(int arg1);

    void on_tlsBtn_clicked();

    void on_standardBox_currentIndexChanged(int index);

private:
    Ui::Home_DevWid *ui;
    Core_Http *mHttp;
    mCoreUnit *mIt;
    sDataPacket *mPro;
    sCfgComIt *item;
};

#endif // HOME_DEVWID_H
