#ifndef HOME_WORKWID_H
#define HOME_WORKWID_H

#include <QWidget>
#include "logmainwid.h"
#include "cfg_app.h"

namespace Ui {
class Home_WorkWid;
}

class Home_WorkWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_WorkWid(QWidget *parent = nullptr);
    ~Home_WorkWid();

signals:
    void startSig();
    void downSig(const QString &dir);
    void msgSig(const QString &msg);

private:
    bool workDown();
    bool checkInput();
    bool startZip();
    bool rsaSig(const QString &fn);
    void writeLog(const sAppVerIt &app);
    bool packing(Cfg_App &cfg, const QStringList &apps);

private slots:
    void on_selectBtn_clicked();
    void on_startBtn_clicked();
    void on_seeBtn_clicked();
    void on_checkBtn_clicked();

private:
    Ui::Home_WorkWid *ui;
};

#endif // HOME_WORKWID_H
