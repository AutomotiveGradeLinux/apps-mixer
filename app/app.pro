TARGET = mixer
QT = quickcontrols2

HEADERS += \
    pacontrolmodel.h \
    pac.h

SOURCES = main.cpp \
    pacontrolmodel.cpp \
    pac.c

CONFIG += link_pkgconfig
PKGCONFIG += libpulse

RESOURCES += \
    Mixer.qrc

include(app.pri)
