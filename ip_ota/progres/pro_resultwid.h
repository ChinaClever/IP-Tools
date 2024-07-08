#ifndef PRO_RESULTWID_H
#define PRO_RESULTWID_H

#include "pro_statuswid.h"
#include <QPlainTextEdit>

namespace Ui {
class Pro_ResultWid;
}

class Pro_ResultWid : public QWidget
{
    Q_OBJECT

public:
    explicit Pro_ResultWid(QWidget *parent = nullptr);
    ~Pro_ResultWid();

public slots:
    void startSlot();

private slots:
    void initFunSlot();
    void finishSlot(bool, const QString &);
    void upgradeError(const QString &host, const QString &msg);
    void insertText(QPlainTextEdit *textEdit, const QString &str);

private:
    Ui::Pro_ResultWid *ui;
};

#endif // PRO_RESULTWID_H
