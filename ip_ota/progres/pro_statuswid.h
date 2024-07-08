#ifndef PRO_STATUSWID_H
#define PRO_STATUSWID_H
#include "home_mainwid.h"
#include "pro_tablewid.h"

namespace Ui {
class Pro_StatusWid;
}

class Pro_StatusWid : public QWidget
{
    Q_OBJECT

public:
    explicit Pro_StatusWid(QWidget *parent = nullptr);
    ~Pro_StatusWid();

public slots:
    void startSlot();
    void initFunSlot();
    void updateIps(const QStringList& ips);
    void subProgressSlot(const QString &ip, int);

private:
    Ui::Pro_StatusWid *ui;
    QMap<QString, int> mMap;
    Pro_TableWid *mTable;
};

#endif // PRO_STATUSWID_H
