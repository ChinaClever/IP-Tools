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
    void updateOutlets();
    void updateThresholds();
    QList<QCheckBox *> checkBoxFound();


private slots:
    void on_uncheckBtn_clicked();
    void on_eleClearBtn_clicked();
    void on_factoryBtn_clicked();
    void on_lineVolBox_valueChanged(int arg1);
    void on_lineCurBox_valueChanged(int arg1);
    void on_loopVolBox_valueChanged(int arg1);
    void on_loopCurBox_valueChanged(int arg1);

    void on_logBtn_clicked();
    void on_cascadeBtn_clicked();
    void on_openAllBtn_clicked();
    void on_closeAllBtn_clicked();
    void on_enDelayBtn_clicked();
    void on_disDelayBtn_clicked();

    void on_rtuEnBtn_clicked();

    void on_rtuDisenBtn_clicked();

    void on_bosEnBtn_clicked();

    void on_boxDisenBtn_clicked();

private:
    Ui::Home_DevWid *ui;
    Core_Http *mHttp;
    sCoreUnit *mIt;
    sDataPacket *mPro;
};

#endif // HOME_DEVWID_H
