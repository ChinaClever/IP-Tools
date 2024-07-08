#include "mainwindow.h"
#include "msgbox.h"
#include <QApplication>
#include "qtsingleapplication.h"

int main(int argc, char *argv[])
{
    QtSingleApplication a(QLatin1String("Pro-QC"),argc, argv);
    if(a.isRunning()) {
        MsgBox::critical(NULL, QObject::tr("质检工具已运行！"));
        a.sendMessage("raise_window_noop", 1000); //4s后激活前个实例
        return EXIT_SUCCESS;
    } else {
        QCoreApplication::setOrganizationName("CLEVER");
        QCoreApplication::setOrganizationDomain("clever.com");
        QCoreApplication::setApplicationName("Pro-QC");
    }
    MainWindow w;
    w.show();
    return a.exec();
}