#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T23:11:00
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "$$PWD/../../include"

win32:INCLUDEPATH += "$$(LIBSPOTIFY_PATH)/include"

win32:LIBS += -L$$(LIBSPOTIFY_PATH)/lib -llibspotify
unix:LIBS += -lspotify

win32:PRE_TARGETDEPS += $$PWD/../../lib/spotinettad.lib

LIBS += -L$$PWD/../../lib -lspotinettad


SOURCES += main.cpp \
    spotinettaconsole.cpp

HEADERS += \
    spotinettaconsole.h
