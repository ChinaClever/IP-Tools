#ifndef Pro_TableWid_H
#define Pro_TableWid_H

#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include "backcolourcom.h"
#include <QTableWidgetItem>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QDateTime>

namespace Ui {
class Pro_TableWid;
}

class Pro_TableWid : public QWidget
{
    Q_OBJECT

public:
    explicit Pro_TableWid(QWidget *parent = 0);
    ~Pro_TableWid();

    int getList(QList<QStringList> list);

    void initTableWid(QStringList &header, int line);
    void setTableItem(int id, int column, const QString &str);
    void setTableRow(int id, QStringList &listStr);
    void setTableRow(int id, int col, QStringList &listStr);
    void appendTableRow(QStringList &listStr);
    void checkTableRow(int line);

    void insertRow(int id, QStringList &listStr);
    void clearRow(int row);
    void clearTable();
    void delTable();
    void setAlarmBackgroundColor(int id);
    void setNormalBackgroundColor(int id);
    void setItemColor(int id, int column, int alarm);
    void scrollToBottomTable();
    void setColumnWidth(int column, int width);
    void setAlignLeft(int row, int col);

    int currentRow();
    int currentColumn();

public slots:
    virtual void timeoutDone(){}
    virtual void itemDoubleSlot(){}
    virtual void itemDoubleClicked(QTableWidgetItem *);

protected:
    QTimer *timer;

private:
    void initScrollArea();
    bool eventFilter(QObject *obj, QEvent *event);
    void initTableWidget(QStringList &header);
    void addItemContent(int row, int column, const QString &content);
    void addRowContent(int row, QStringList &list);
    void addInitRow(int row);
    void addTableRows(int line);
    void delTableRows(int line);

private:
    Ui::Pro_TableWid *ui;
    QStringList mHeader;
    QScrollBar *m_scrollBarV;
};

#endif // Pro_TableWid_H
