#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

QT       += core gui printsupport svg #multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = polyphone
TEMPLATE = app

CONFIG(release, debug|release) {
    DESTDIR = RELEASE
    OBJECTS_DIR = RELEASE/.obj
    MOC_DIR = RELEASE/.moc
    RCC_DIR = RELEASE/.rcc
    UI_DIR = RELEASE/.ui
}
CONFIG(debug, debug|release) {
    DESTDIR = DEBUG
    OBJECTS_DIR = DEBUG/.obj
    MOC_DIR = DEBUG/.moc
    RCC_DIR = DEBUG/.rcc
    UI_DIR = DEBUG/.ui
}

LIBS +=  -Lsynthetiseur/jack \
    -ljack

win32 {
    QT += multimedia
    DEFINES += __WINDOWS_MM__
    CONFIG += use_asio
    LIBS += -lwinmm
}
unix {
    CONFIG += mobility
    MOBILITY += multimedia
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa
    INCLUDEPATH += /usr/include/jack
}

INCLUDEPATH += gui_divers \
    pages \
    qcustomplot \
    ressources \
    sf2_core \
    tools \
    clavier \
    synthetiseur \
    synthetiseur/elements \
    .

SOURCES	+= main.cpp \
    mainwindow.cpp \
    sf2_core/sf2_types.cpp \
    sf2_core/tree.cpp \
    sf2_core/pile_sf2.cpp \
    sf2_core/pile_actions.cpp \
    sf2_core/sound.cpp \
    sf2_core/pile_sf2_sl.cpp \
    gui_divers/config.cpp \
    gui_divers/dialog_list.cpp \
    gui_divers/dialog_help.cpp \
    pages/page_sf2.cpp \
    pages/page_smpl.cpp \
    pages/page_inst.cpp \
    pages/page_prst.cpp \
    pages/page.cpp \
    qcustomplot/qcustomplot.cpp \
    tools/dialog_space.cpp \
    tools/dialog_paramglobal.cpp \
    tools/dialog_mixture.cpp \
    tools/dialog_sifflements.cpp \
    clavier/RtMidi.cpp \
    clavier/pianoscene.cpp \
    clavier/pianokey.cpp \
    clavier/pianokeybd.cpp \
    clavier/keylabel.cpp \
    clavier/pianokeybdcustom.cpp \
    synthetiseur/synth.cpp \
    synthetiseur/voice.cpp \
    synthetiseur/circularbuffer.cpp \
    synthetiseur/voiceparam.cpp \
    synthetiseur/elements/enveloppevol.cpp \
    synthetiseur/audiodevice.cpp \
    synthetiseur/elements/oscsinus.cpp

HEADERS  += mainwindow.h \
    sf2_core/sf2_types.h \
    sf2_core/tree.h \
    sf2_core/pile_sf2.h \
    sf2_core/pile_actions.h \
    sf2_core/sound.h \
    gui_divers/config.h \
    gui_divers/dialog_list.h \
    gui_divers/dialog_help.h \
    pages/page_sf2.h \
    pages/page_smpl.h \
    pages/page_inst.h \
    pages/page_prst.h \
    pages/page.h \
    qcustomplot/qcustomplot.h \
    tools/dialog_space.h \
    tools/dialog_paramglobal.h \
    tools/dialog_mixture.h \
    tools/dialog_sifflements.h \
    clavier/RtMidi.h \
    clavier/RtError.h \
    clavier/pianoscene.h \
    clavier/pianokey.h \
    clavier/keyboardmap.h \
    clavier/keylabel.h \
    clavier/pianokeybd.h \
    clavier/pianokeybdcustom.h \
    synthetiseur/synth.h \
    synthetiseur/voice.h \
    synthetiseur/circularbuffer.h \
    synthetiseur/voiceparam.h \
    synthetiseur/elements/enveloppevol.h \
    synthetiseur/audiodevice.h \
    synthetiseur/elements/oscsinus.h

FORMS    += mainwindow.ui \
    gui_divers/config.ui \
    gui_divers/dialog_list.ui \
    gui_divers/dialog_help.ui \
    pages/page_sf2.ui \
    pages/page_smpl.ui \
    pages/page_inst.ui \
    pages/page_prst.ui \
    tools/dialog_space.ui \
    tools/dialog_paramglobal.ui \
    tools/dialog_mixture.ui \
    tools/dialog_sifflements.ui

RESOURCES += ressources.qrc \
    clavier/pianokeybd.qrc
RC_FILE = polyphone.rc

TRANSLATIONS = polyphone_en.ts

# si ASIO activé (CONFIG += use_asio)
use_asio{
    LIBS += libole32
    DEFINES += PA_USE_ASIO
    DEFINES -= UNICODE
    INCLUDEPATH += synthetiseur/portaudio/include \
        synthetiseur/portaudio/src/common \
        synthetiseur/portaudio/src/os/win \
        synthetiseur/ASIOSDK2/common \
        synthetiseur/ASIOSDK2/host \
        synthetiseur/ASIOSDK2/host/pc
    SOURCES += synthetiseur/ASIOSDK2/common/asio.cpp \
        synthetiseur/ASIOSDK2/host/asiodrivers.cpp \
        synthetiseur/ASIOSDK2/host/pc/asiolist.cpp \
        synthetiseur/portaudio/src/hostapi/asio/pa_asio.cpp \
        synthetiseur/portaudio/src/common/pa_trace.c \
        synthetiseur/portaudio/src/common/pa_stream.c \
        synthetiseur/portaudio/src/common/pa_ringbuffer.c \
        synthetiseur/portaudio/src/common/pa_process.c \
        synthetiseur/portaudio/src/common/pa_front.c \
        synthetiseur/portaudio/src/common/pa_dither.c \
        synthetiseur/portaudio/src/common/pa_cpuload.c \
        synthetiseur/portaudio/src/common/pa_converters.c \
        synthetiseur/portaudio/src/common/pa_allocation.c \
        synthetiseur/portaudio/src/os/win/pa_x86_plain_converters.c \
        synthetiseur/portaudio/src/os/win/pa_win_waveformat.c \
        synthetiseur/portaudio/src/os/win/pa_win_util.c \
        synthetiseur/portaudio/src/os/win/pa_win_hostapis.c \
        synthetiseur/portaudio/src/os/win/pa_win_coinitialize.c \
        synthetiseur/portaudio/src/hostapi/asio/iasiothiscallresolver.cpp
}
