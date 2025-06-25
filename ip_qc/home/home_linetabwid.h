#ifndef HOME_LINETABWID_H
#define HOME_LINETABWID_H

#include "comtablewid.h"
#include "datapacket.h"
#include "home_devwid.h"

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
