INCLUDEPATH += $$PWD

win32: {
    QT += axcontainer
#    LIBS += -L$$PWD/lib/ -licom
#    CONFIG += qaxcontainer
} else: {
}

# 日志导出后端。默认 QXlsx，不依赖本机安装 Excel。
# 改回 Windows OLE/QExcel：注释 EXCEL_USE_QXLSX，并取消 EXCEL_USE_OLE 的注释。
DEFINES += EXCEL_USE_QXLSX
# DEFINES += EXCEL_USE_OLE

include($$PWD/QXlsx/QXlsx.pri)

DEPENDPATH += $$PWD


HEADERS += \
    $$PWD/qexcel.h \
    $$PWD/excel_savethread.h

SOURCES += \
    $$PWD/qexcel.cpp \
    $$PWD/excel_savethread.cpp
