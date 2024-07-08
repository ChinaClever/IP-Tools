#ifndef HOME_STATEWID_H
#define HOME_STATEWID_H

#include "comtablewid.h"
#include "core_sender.h"

namespace Ui {
class Home_StateWid;
}

class Home_StateWid : public QWidget
{
    Q_OBJECT

public:
    explicit Home_StateWid(QWidget *parent = nullptr);
    ~Home_StateWid();

public slots:
    void startSlot();
    void setTextColor(const QString &str);

private slots:
    void initFunSlot();
    void insertSlot(const QString &str);
    void finishSlot(bool pass, const QString &msg);
    void upgradeError(const QString &host, const QString &msg);

private:
    Ui::Home_StateWid *ui;
    int mCnt;
};

#endif // HOME_STATEWID_H
