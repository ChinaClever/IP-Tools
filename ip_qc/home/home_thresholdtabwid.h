#ifndef HOME_THRESHOLDTABWID_H
#define HOME_THRESHOLDTABWID_H

#include "home_linetabwid.h"

class Home_ThresholdTabWid : public ComTableWid
{
    Q_OBJECT
public:
    explicit Home_ThresholdTabWid(QWidget *parent = nullptr);

signals:

protected:
    void initWid();
    void appendItem();

protected slots:
    void timeoutDone();

private:
};

#endif // HOME_THRESHOLDTABWID_H
