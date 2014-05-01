#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T20:32:48
#
#-------------------------------------------------

QT       -= gui

TARGET = spotinetta
TEMPLATE = lib

CONFIG += staticlib c++11 debug_and_release build_all

RCC_DIR = "build/rcc"
UI_DIR = "build/uic"
MOC_DIR = "build/moc"
OBJECTS_DIR = "build/obj"

DESTDIR = "lib"

CONFIG(debug, debug|release) {
    DESTDIR = "lib"
    TARGET = "spotinettad"
}
CONFIG(release, debug|release) {
    DESTDIR = "lib"
    TARGET = "spotinetta"
}

INCLUDEPATH -= "$$PWD"
INCLUDEPATH += "$$PWD/include"
win32:INCLUDEPATH += "$$(LIBSPOTIFY_PATH)/include"

win32:LIBS += -L$$(LIBSPOTIFY_PATH)/lib -llibspotify
unix:LIBS += -lspotify

SOURCES += \
    src/session.cpp \
    src/objects/track.cpp \
    src/objects/search.cpp \
    src/objects/link.cpp \
    src/objects/image.cpp \
    src/objects/artist.cpp \
    src/objects/album.cpp \
    src/objects/playlist.cpp \
    src/objects/user.cpp \
    src/objects/playlistcontainer.cpp \
    src/watchers/playlistcontainerwatcher.cpp \
    src/watchers/playlistwatcher.cpp \
    src/objects/artistbrowse.cpp \
    src/objects/albumbrowse.cpp \
    src/watchers/imagewatcher.cpp

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
    src/events.h \
    include/Spotinetta/playlist.h \
    include/Spotinetta/user.h \
    include/Spotinetta/playlistcontainer.h \
    include/Spotinetta/playlistcontainerwatcher.h \
    include/Spotinetta/playlistwatcher.h \
    include/Spotinetta/watchers.h \
    include/Spotinetta/detail/ringbuffer.h \
    include/Spotinetta/audiooutput.h \
    include/Spotinetta/basicwatcher.h \
    include/Spotinetta/artistbrowse.h \
    include/Spotinetta/albumbrowse.h \
    include/Spotinetta/objects.h \
    include/Spotinetta/Spotinetta \
    include/Spotinetta/imagewatcher.h
