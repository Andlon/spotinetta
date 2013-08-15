#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T20:32:48
#
#-------------------------------------------------

QT       -= gui

TARGET = spotinetta
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/session.cpp \
    src/objects/track.cpp \
    src/objects/search.cpp \
    src/objects/link.cpp \
    src/objects/image.cpp \
    src/objects/artist.cpp \
    src/objects/album.cpp

HEADERS += \
    include/Spotinetta/track.h \
    include/Spotinetta/session.h \
    include/Spotinetta/search.h \
    include/Spotinetta/link.h \
    include/Spotinetta/image.h \
    include/Spotinetta/error.h \
    include/Spotinetta/definitions.h \
    include/Spotinetta/basicobject.h \
    include/Spotinetta/artist.h \
    include/Spotinetta/album.h \
    src/events.h
