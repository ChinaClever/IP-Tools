#ifndef HOME_LINETABWID_H
#define HOME_LINETABWID_H

#include "comtablewid.h"
#include "home_workwid.h"
#include "core_thread.h"

class Home_LineTabWid : public ComTableWid
{
    Q_OBJECT
public:
    explicit Home_LineTabWid(QWidget *parent = nullptr);

signals:

protected:
    void initWid();
    void appendItem();

protected slots:
    void timeoutDone();

};

#endif // HOME_LINETABWID_H
