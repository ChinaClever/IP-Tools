#ifndef EXCEL_SAVETHREAD_H
#define EXCEL_SAVETHREAD_H

#include <QtCore>

/*
 * 导出实现由编译宏切换，外部接口不变：
 *   EXCEL_USE_QXLSX  使用 QXlsx（默认，见 excel.pri）
 *   EXCEL_USE_OLE    使用 Windows QExcel / OLE
 */
#if !defined(EXCEL_USE_QXLSX) && !defined(EXCEL_USE_OLE)
#define EXCEL_USE_QXLSX
#endif

class Excel_SaveThread : public QThread
{
    Q_OBJECT
public:
    explicit Excel_SaveThread(QObject *parent = nullptr);
    ~Excel_SaveThread();

    int getProgress();
    void saveData(const QString &fileName, QList<QStringList> &list);
protected:
    void run();
    bool init();
    void writeFile(QList<QStringList> &list);

private:
    QString mFileName;
    int mSize, mAllSize;
    QList<QStringList> mList;
};

#endif // EXCEL_SAVETHREAD_H
