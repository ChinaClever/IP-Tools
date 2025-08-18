QT += core gui printsupport
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/printer_bartender.h

SOURCES += \
    $$PWD/printer_bartender.cpp

win32:LIBS += -lwinspool
