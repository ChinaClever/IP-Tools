#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "navbarwid.h"
#include "home_mainwid.h"
#include "remote_swverwid.h"
#include "pro_resultwid.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void initFunSlot();
    void navBarSlot(int);

private:
    Ui::MainWindow *ui;
    NavBarWid *mNavBarWid;
    Home_MainWid *mHomeWid;
    Remote_RecvWid *mRecvWid;
    Remote_SwVerWid *mSwWid;
    Pro_StatusWid *mStatus;
    Pro_ResultWid *mResult;
};
#endif // MAINWINDOW_H
