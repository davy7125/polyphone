TEMPLATE = lib
TARGET = vpianokeybd
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
QT += core \
    gui \
    svg
CONFIG += designer \
    plugin \
    release
DEFINES += VPIANO_PLUGIN
linux* {
    CONFIG += x11
}
win32 { 
    CONFIG -= debug
    CONFIG -= debug_and_release
}
macx {
    LIBS += -framework Carbon
}
HEADERS += pianoscene.h \
    pianokey.h \
    pianokeybd.h \
    plugin.h \
    keyboardmap.h \
    keylabel.h \
    pianodefs.h \
    rawkeybdapp.h
SOURCES += pianoscene.cpp \
    pianokey.cpp \
    pianokeybd.cpp \
    plugin.cpp \
    keylabel.cpp \
    rawkeybdapp.cpp
RESOURCES += pianokeybd.qrc
