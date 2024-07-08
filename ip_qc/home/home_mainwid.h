#ifndef HOME_MAINWID_H
#define HOME_MAINWID_H

#include <QWidget>
#include "home_workwid.h"


namespace Ui {
class Home_MainWid;
}

class Home_MainWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_MainWid(QWidget *parent = nullptr);
    ~Home_MainWid();

signals:
    void startSig();
    void setDevSig();

protected:
    void initWid();
    void setTextColor(const QString &str);

private slots:
    void onStart();
    void onMsg(const QString &msg);

private:
    Ui::Home_MainWid *ui;
    Home_WorkWid *mWorkWid;
    int mId=0;
    //Td_MainWid *mTabWid;
};

#endif // HOME_MAINWID_H
