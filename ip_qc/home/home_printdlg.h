#ifndef HOME_PRINTDLG_H
#define HOME_PRINTDLG_H

#include <QDialog>
#include "printer/printer_bartender.h"
#include "cfgcom.h"


namespace Ui {
class Home_PrintDlg;
}

class Home_PrintDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Home_PrintDlg(QWidget *parent = nullptr);
    ~Home_PrintDlg();

protected slots:
    void initFunSlot();

private slots:
    void on_testPrintBtn_clicked();

    void on_saveBtn_clicked();

    void on_closeBtn_clicked();

    // void on_testPrintInfoBtn_clicked();

private:
    Ui::Home_PrintDlg *ui;
    sCfgComIt *mItem;
};

#endif // HOME_PRINTDLG_H
