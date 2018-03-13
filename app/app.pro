TARGET = mixer
QT = quickcontrols2 websockets core

HEADERS += \
    mixer.h \
    qafbwsmsg.h \
    qafbwsclient.h

SOURCES = main.cpp \
    mixer.cpp \
    qafbwsclient.cpp \
    qafbwsmsg.cpp

CONFIG += link_pkgconfig
PKGCONFIG += libhomescreen qlibwindowmanager libpulse

RESOURCES += \
    Mixer.qrc

include(app.pri)
