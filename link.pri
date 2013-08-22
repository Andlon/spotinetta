# Include this file to link Spotinetta to your project. Example:
#
# In your qmake project file (.pro):
#   include("path/to/link.pri")
#


# Add necessary libspotify include and lib
# Note that on win32 I've chosen to let an environment variable called
# LIBSPOTIFY_PATH determine where to look for libspotify, while on
# Linux, libspotify is easily installed into system lib paths

win32:INCLUDEPATH += "$$(LIBSPOTIFY_PATH)/include"
win32:LIBS += -L$$(LIBSPOTIFY_PATH)/lib -llibspotify
unix:LIBS += -lspotify

# Add necessary includes and lib for Spotinetta. Need to improve this,
# especially for Linux, which atm is untested
INCLUDEPATH += $$PWD/include
win32:PRE_TARGETDEPS += $$PWD/lib/spotinettad.lib

LIBS += -L$$PWD/lib

CONFIG(debug, debug|release) {
    LIBS += -lspotinettad
}
CONFIG(release, debug|release) {
    LIBS += -lspotinetta
}
