INCLUDEPATH += $$PWD

include($$PWD/jcon/jcon.pri)

HEADERS += \
    $$PWD/jsonrpcclient.h \
    $$PWD/jsonrpcobj.h

SOURCES += \
    $$PWD/jsonrpcclient.cpp \
    $$PWD/jsonrpcobj.cpp
