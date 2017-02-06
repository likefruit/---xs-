#-------------------------------------------------
#
# Project created by QtCreator 2013-05-27T18:13:30
#
#-------------------------------------------------

QT       -= core gui

TARGET = mwlib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    mwlib.c

HEADERS += mwlib.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
