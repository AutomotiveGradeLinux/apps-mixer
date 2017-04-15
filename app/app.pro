TARGET = mixer
QT = quickcontrols2

HEADERS += \
    pacontrolmodel.h \
    paclient.h

SOURCES = main.cpp \
    pacontrolmodel.cpp \
    paclient.cpp

CONFIG += link_pkgconfig
PKGCONFIG += libpulse

RESOURCES += \
    Mixer.qrc

include(app.pri)
