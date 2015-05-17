#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

# Option for developers ONLY
#DEFINES += SHOW_ID_ERROR

# Use local copies of RtMidi, Stk and QCustomplot libraries
# (this is forced to true for Windows or Mac OS X)
#DEFINES += USE_LOCAL_LIBRARIES


QT       += core gui printsupport svg

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
    DEFINES += __WINDOWS_MM__ USE_LOCAL_LIBRARIES
    INCLUDEPATH += lib/win/ lib
    HEADERS  += lib/win/zconf.h \
        lib/win/zlib.h \
        lib/portaudio.h
    RC_FILE = polyphone.rc

    !contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += -Llib/win/32bits -lportaudio_x86 -lzlib1 -lwinmm
    } else {
        LIBS += -Llib/win/64bits -lportaudio_x64 -lzlib1 -lwinmm
    }
}
unix:!macx{
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa jack portaudio-2.0 zlib
    INCLUDEPATH += /usr/include/jack
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
    QMAKE_MAC_SDK = macosx10.9
    DEFINES += __MACOSX_CORE__ USE_LOCAL_LIBRARIES
    INCLUDEPATH += lib/mac/Jackmp.framework/Headers \
        lib
    HEADERS += lib/portaudio.h
    LIBS += -L$$PWD/lib/mac -lportaudio -F$$PWD/lib/mac/ -framework Jackmp \
        -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework AudioUnit -framework AudioToolbox -framework Cocoa -lz
    ICON = polyphone.icns
    QMAKE_INFO_PLIST = polyphone.plist
}

# location of RtMidi, Stk and QCustomplot
contains(DEFINES, USE_LOCAL_LIBRARIES) {
    HEADERS += lib/rtmidi/RtMidi.h \
        lib/stk/Stk.h \
        lib/stk/SineWave.h \
        lib/stk/OnePole.h \
        lib/stk/Generator.h \
        lib/stk/Filter.h \
        lib/stk/Effect.h \
        lib/stk/Delay.h \
        lib/stk/Chorus.h \
        lib/stk/FreeVerb.h \
        lib/stk/DelayL.h \
        lib/qcustomplot/qcustomplot.h
    SOURCES += lib/rtmidi/RtMidi.cpp \
        lib/stk/Stk.cpp \
        lib/stk/SineWave.cpp \
        lib/stk/OnePole.cpp \
        lib/stk/Delay.cpp \
        lib/stk/Chorus.cpp \
        lib/stk/FreeVerb.cpp \
        lib/stk/DelayL.cpp \
        lib/qcustomplot/qcustomplot.cpp
    INCLUDEPATH += lib/rtmidi \
        lib/stk \
        lib/qcustomplot
} else {
    PKGCONFIG += rtmidi
    LIBS += -lstk -lqcustomplot
    INCLUDEPATH += /usr/include/stk \
        /usr/include/qcustomplot
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
    sfark \
    .

SOURCES	+= main.cpp \
    mainwindow.cpp \
    sf2_core/sf2_types.cpp \
    sf2_core/tree.cpp \
    sf2_core/pile_sf2.cpp \
    sf2_core/pile_actions.cpp \
    sf2_core/sound.cpp \
    sf2_core/pile_sf2_sl.cpp \
    sf2_core/duplicator.cpp \
    sf2_core/treewidgetitem.cpp \
    gui_divers/config.cpp \
    gui_divers/dialog_list.cpp \
    gui_divers/dialog_help.cpp \
    gui_divers/dialog_rename.cpp \
    gui_divers/editkey.cpp \
    gui_divers/tablekey.cpp \
    gui_divers/dialog_about.cpp \
    gui_divers/dialog_export.cpp \
    gui_divers/spinboxkey.cpp \
    gui_divers/spinboxrange.cpp \
    pages/page_sf2.cpp \
    pages/page_smpl.cpp \
    pages/page_inst.cpp \
    pages/page_prst.cpp \
    pages/page.cpp \
    pages/graphique.cpp \
    pages/graphiquefourier.cpp \
    pages/tablewidgetmod.cpp \
    pages/pagetable.cpp \
    pages/tablewidget.cpp \
    tools/dialog_space.cpp \
    tools/dialog_paramglobal.cpp \
    tools/dialog_mixture.cpp \
    tools/dialog_sifflements.cpp \
    tools/dialog_release.cpp \
    tools/dialog_selectitems.cpp \
    tools/dialog_magnetophone.cpp \
    tools/dialog_celeste.cpp \
    tools/dialog_visualizer.cpp \
    tools/dialog_duplication.cpp \
    tools/conversion_sfz.cpp \
    tools/import_sfz.cpp \
    clavier/pianoscene.cpp \
    clavier/pianokey.cpp \
    clavier/pianokeybd.cpp \
    clavier/keylabel.cpp \
    clavier/pianokeybdcustom.cpp \
    clavier/keyboardmap.cpp \
    synthetiseur/synth.cpp \
    synthetiseur/voice.cpp \
    synthetiseur/circularbuffer.cpp \
    synthetiseur/voiceparam.cpp \
    synthetiseur/audiodevice.cpp \
    synthetiseur/soundengine.cpp \
    synthetiseur/elements/calibrationsinus.cpp \
    synthetiseur/elements/enveloppevol.cpp \
    synthetiseur/elements/oscsinus.cpp \
    sfark/sfarkglobal.cpp \
    sfark/sfarkfilemanager.cpp \
    gui_divers/dialogselection.cpp \
    pages/rectangleitem.cpp \
    pages/graphicsviewrange.cpp \
    tools/dialog_transposition.cpp

HEADERS  += mainwindow.h \
    sf2_core/sf2_types.h \
    sf2_core/tree.h \
    sf2_core/pile_sf2.h \
    sf2_core/pile_actions.h \
    sf2_core/sound.h \
    sf2_core/duplicator.h \
    sf2_core/treewidgetitem.h \
    gui_divers/config.h \
    gui_divers/dialog_list.h \
    gui_divers/dialog_help.h \
    gui_divers/dialog_rename.h \
    gui_divers/editkey.h \
    gui_divers/tablekey.h \
    gui_divers/dialog_about.h \
    gui_divers/dialog_export.h \
    gui_divers/spinboxkey.h \
    gui_divers/spinboxrange.h \
    pages/page_sf2.h \
    pages/page_smpl.h \
    pages/page_inst.h \
    pages/page_prst.h \
    pages/page.h \
    pages/graphique.h \
    pages/graphiquefourier.h \
    pages/tablewidgetmod.h \
    pages/pagetable.h \
    pages/tablewidget.h \
    tools/dialog_space.h \
    tools/dialog_paramglobal.h \
    tools/dialog_mixture.h \
    tools/dialog_sifflements.h \
    tools/dialog_release.h \
    tools/dialog_selectitems.h \
    tools/dialog_magnetophone.h \
    tools/dialog_celeste.h \
    tools/dialog_visualizer.h \
    tools/dialog_duplication.h \
    tools/conversion_sfz.h \
    tools/import_sfz.h \
    clavier/pianoscene.h \
    clavier/pianokey.h \
    clavier/keyboardmap.h \
    clavier/keylabel.h \
    clavier/pianokeybd.h \
    clavier/pianokeybdcustom.h \
    clavier/keyboardmap.h \
    synthetiseur/synth.h \
    synthetiseur/voice.h \
    synthetiseur/circularbuffer.h \
    synthetiseur/voiceparam.h \
    synthetiseur/audiodevice.h \
    synthetiseur/soundengine.h \
    synthetiseur/elements/calibrationsinus.h \
    synthetiseur/elements/enveloppevol.h \
    synthetiseur/elements/oscsinus.h \
    macapplication.h \
    sfark/sfarkglobal.h \
    sfark/sfarkfilemanager.h \
    sfark/sfarkextractor.h \
    gui_divers/dialogselection.h \
    pages/combobox.h \
    pages/tablecombobox.h \
    pages/graphicssimpletextitem.h \
    pages/rectangleitem.h \
    pages/graphicsviewrange.h \
    tools/dialog_transposition.h

FORMS    += mainwindow.ui \
    gui_divers/config.ui \
    gui_divers/dialog_list.ui \
    gui_divers/dialog_rename.ui \
    gui_divers/dialog_about.ui \
    gui_divers/dialog_export.ui \
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
    tools/dialog_magnetophone.ui \
    tools/dialog_celeste.ui \
    tools/dialog_visualizer.ui \
    tools/dialog_duplication.ui \
    gui_divers/dialogselection.ui \
    tools/dialog_transposition.ui

# Special compiler for sfark extractor
SPECIAL_SOURCE = sfark/sfarkextractor.cpp
ExtraCompiler.input = SPECIAL_SOURCE
ExtraCompiler.variable_out = OBJECTS
ExtraCompiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${QMAKE_EXT_OBJ}
win32{
    ExtraCompiler.commands = $${QMAKE_CXX} $(CXXFLAGS) -arch:IA32 -D_CRT_SECURE_NO_WARNINGS $(INCPATH) -c ${QMAKE_FILE_IN} -Fo${QMAKE_FILE_OUT}
}
unix:!macx{
    ExtraCompiler.commands = $${QMAKE_CXX} -fPIC -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
macx {
    ExtraCompiler.commands = $${QMAKE_CXX} $(CXXFLAGS) -mno-sse $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
QMAKE_EXTRA_COMPILERS += ExtraCompiler

RESOURCES += ressources.qrc \
    clavier/pianokeybd.qrc

TRANSLATIONS = polyphone_en.ts \
    polyphone_es.ts \
    polyphone_it.ts
