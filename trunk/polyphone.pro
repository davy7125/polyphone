#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

QT       += core gui printsupport svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Option for developers ONLY
DEFINES += SHOW_ID_ERROR

TARGET = polyphone
TEMPLATE = app

CONFIG(release, debug|release){
    DESTDIR = RELEASE
    OBJECTS_DIR = RELEASE/.obj
    MOC_DIR = RELEASE/.moc
    RCC_DIR = RELEASE/.rcc
    UI_DIR = RELEASE/.ui
}
CONFIG(debug, debug|release){
    DESTDIR = DEBUG
    OBJECTS_DIR = DEBUG/.obj
    MOC_DIR = DEBUG/.moc
    RCC_DIR = DEBUG/.rcc
    UI_DIR = DEBUG/.ui
}

win32{
    DEFINES += __WINDOWS_MM__ PA_USE_ASIO
    LIBS += -Llib/win -lportaudio -ljack \
            -lwinmm libole32
    INCLUDEPATH += lib/win/
    HEADERS  += lib/win/jack.h \
        lib/win/weakmacros.h \
        lib/win/types.h \
        lib/win/transport.h \
        lib/win/systemdeps.h \
        lib/win/session.h
}
unix{
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa
    INCLUDEPATH += /usr/include/jack
    LIBS += -ljack # Fedora: -libportaudio

    contains(QMAKE_HOST.arch, x86_64):{
        LIBS += /usr/lib/x86_64-linux-gnu/libportaudio.so.2
    } else {
        LIBS += /usr/lib/i386-linux-gnu/libportaudio.so.2
    }
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
    lib \
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
    tools/dialog_release.cpp \
    tools/dialog_selectitems.cpp \
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
    synthetiseur/elements/oscsinus.cpp \
    synthetiseur/elements/Stk.cpp \
    synthetiseur/elements/SineWave.cpp \
    synthetiseur/elements/OnePole.cpp \
    synthetiseur/elements/FreeVerb.cpp \
    synthetiseur/elements/Delay.cpp \
    synthetiseur/elements/Chorus.cpp \
    synthetiseur/elements/DelayL.cpp \
    gui_divers/dialog_rename.cpp \
    tools/dialog_magnetophone.cpp \
    tools/dialog_celeste.cpp \
    tools/dialog_visualizer.cpp \
    tools/dialog_duplication.cpp \
    clavier/keymapper.cpp \
    gui_divers/editkey.cpp \
    gui_divers/tablekey.cpp \
    tools/conversion_sfz.cpp \
    gui_divers/dialog_about.cpp \
    gui_divers/dialog_export.cpp \
    sf2_core/duplicator.cpp \
    tools/import_sfz.cpp

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
    tools/dialog_release.h \
    tools/dialog_selectitems.h \
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
    synthetiseur/elements/oscsinus.h \
    lib/portaudio.h \
    synthetiseur/elements/Stk.h \
    synthetiseur/elements/SineWave.h \
    synthetiseur/elements/OnePole.h \
    synthetiseur/elements/Generator.h \
    synthetiseur/elements/FreeVerb.h \
    synthetiseur/elements/Filter.h \
    synthetiseur/elements/Effect.h \
    synthetiseur/elements/Delay.h \
    synthetiseur/elements/Chorus.h \
    synthetiseur/elements/DelayL.h \
    gui_divers/dialog_rename.h \
    tools/dialog_magnetophone.h \
    tools/dialog_celeste.h \
    tools/dialog_visualizer.h \
    tools/dialog_duplication.h \
    clavier/keymapper.h \
    gui_divers/editkey.h \
    gui_divers/tablekey.h \
    tools/conversion_sfz.h \
    gui_divers/dialog_about.h \
    gui_divers/dialog_export.h \
    sf2_core/duplicator.h \
    tools/import_sfz.h

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
    tools/dialog_sifflements.ui \
    tools/dialog_release.ui \
    tools/dialog_selectitems.ui \
    gui_divers/dialog_rename.ui \
    tools/dialog_magnetophone.ui \
    tools/dialog_celeste.ui \
    tools/dialog_visualizer.ui \
    tools/dialog_duplication.ui \
    gui_divers/dialog_about.ui \
    gui_divers/dialog_export.ui

RESOURCES += ressources.qrc \
    clavier/pianokeybd.qrc
RC_FILE = polyphone.rc

TRANSLATIONS = polyphone_en.ts \
    polyphone_es.ts
