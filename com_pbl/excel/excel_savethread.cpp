/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "excel_savethread.h"
#if defined(EXCEL_USE_QXLSX)
#include "xlsxdocument.h"
#elif defined(Q_OS_WIN32)
#include "msgbox.h"
#include "qexcel.h"
#include <windows.h>   // OleInitialize, CoInitializeEx
#include <objbase.h>   // S_OK, S_FALSE
#endif
Excel_SaveThread::Excel_SaveThread(QObject *parent) : QThread(parent)
{
    mSize = mAllSize = 0;
}

Excel_SaveThread::~Excel_SaveThread()
{
    wait();
}

void Excel_SaveThread::saveData(const QString &fileName, QList<QStringList> &list)
{
    mFileName = fileName;
    mList.clear();
    mList.append(list);
    start();
}


/**
 * @brief 百分制来计算
 */
int Excel_SaveThread::getProgress()
{
    int ret = 100;

    if(mSize < mAllSize)
        ret = (mSize *100.0) / mAllSize ;

    return ret;
}


bool Excel_SaveThread::init()
{
    mSize = mAllSize = 0;
#if defined(EXCEL_USE_QXLSX)
    return true;
#elif defined(Q_OS_WIN32)
    HRESULT r = OleInitialize(0);  // 增加头文件 #include <sapi.h>
    if (r != S_OK && r != S_FALSE)
    {
        qWarning("Qt:初始化Ole失败（error %x）",(unsigned int)r);
        MsgBox::critical(0, tr("Qt:初始化Ole失败"));
        return false;
    }
    return true;
#else
    qWarning("Excel: EXCEL_USE_OLE 仅支持 Windows");
    return false;
#endif
}

void Excel_SaveThread::writeFile(QList<QStringList> &list)
{
#if defined(EXCEL_USE_QXLSX)
    QXlsx::Document xlsx;
    xlsx.addSheet(QStringLiteral("Sheet1"));
    xlsx.selectSheet(QStringLiteral("Sheet1"));

    for(int i=0; i<list.size(); ++i)
    {
        for(int j=0; j<list.at(i).size(); ++j)
            xlsx.write(i + 1, j + 1, list.at(i).at(j));
        mSize++;
    }

    if(!xlsx.saveAs(mFileName + ".xlsx"))
        qWarning("QXlsx:保存文件失败 %s", qPrintable(mFileName + ".xlsx"));
#elif defined(Q_OS_WIN32)
    QExcel excel(this);
    bool ret = excel.createNewExcel(mFileName +".xlsx");
    if(ret) {
        excel.selectSheet("Sheet1"); //  Sheet1
        excel.saveAs();

        for(int i=0; i<list.size(); ++i)
        {
            for(int j=0; j<list.at(i).size(); ++j) {
                excel.setCellString(i+1, j+1, list.at(i).at(j));
            }
            mSize++;
            msleep(25);
        }
        excel.save();
    }
#else
    Q_UNUSED(list);
#endif
}

void Excel_SaveThread::run()
{
    bool ret = init();
    if(ret) {
        mAllSize = mList.size();
        writeFile(mList);
    }
}
