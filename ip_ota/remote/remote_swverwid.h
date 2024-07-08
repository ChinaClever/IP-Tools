#ifndef REMOTE_SWVERWID_H
#define REMOTE_SWVERWID_H

#include "remote_recvwid.h"
#include "ssdp_core.h"

namespace Ui {
class Remote_SwVerWid;
}

class Remote_SwVerWid : public QWidget
{
    Q_OBJECT

public:
    explicit Remote_SwVerWid(QWidget *parent = nullptr);
    ~Remote_SwVerWid();

private slots:
    void recvSlot(const sSdpIt &it);
    void on_startBtn_clicked();

private:
    Ui::Remote_SwVerWid *ui;
    Ssdp_Core *mSsdp;
    int mId;
};

#endif // REMOTE_SWVERWID_H
