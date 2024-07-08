#ifndef NAVBARWID_H
#define NAVBARWID_H

#include <QtCore>
#include <QWidget>
#include "usrlanddlg.h"

namespace Ui {
class NavBarWid;
}

class NavBarWid : public QWidget
{
    Q_OBJECT

public:
    explicit NavBarWid(QWidget *parent = nullptr);
    ~NavBarWid();

private:
    Ui::NavBarWid *ui;
};

#endif // NAVBARWID_H
