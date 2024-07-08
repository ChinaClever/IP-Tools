QT      += network
QT      += websockets

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/net_tcpclient.h \
    $$PWD/net_tcpserver.h \
    $$PWD/net_udp.h \
    $$PWD/udprecvsocket.h \
    $$PWD/macaddr.h


SOURCES += \
    $$PWD/net_tcpclient.cpp \
    $$PWD/net_tcpserver.cpp \
    $$PWD/net_udp.cpp \
    $$PWD/udprecvsocket.cpp \
    $$PWD/macaddr.cpp

FORMS += \





