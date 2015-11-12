#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

# Use local copies of RtMidi, Stk and QCustomplot libraries
# (this is forced to true for Windows or Mac OS X)
# Uncomment a line if your distribution doesn't come with some of the following libraries
#DEFINES += USE_LOCAL_RTMIDI
#DEFINES += USE_LOCAL_STK
#DEFINES += USE_LOCAL_QCUSTOMPLOT

# Polyphone version
DEFINES += VERSION=1.7
DEFINES += FINAL=true

# List to complete if other translations are wished
# Format: polyphone_XX.ts where XX is the language code
TRANSLATIONS = polyphone_en.ts \
    polyphone_es.ts \
    polyphone_it.ts \
    polyphone_sr.ts \
    polyphone_de.ts \
    polyphone_cs.ts


QT       += core gui printsupport svg
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

win32{
    DEFINES += __WINDOWS_MM__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += lib \
        lib/win/ \
        lib/ogg_vorbis
    HEADERS  += lib/win/zconf.h \
        lib/win/zlib.h \
        lib/portaudio.h \
        lib/ogg_vorbis/ogg/ogg.h \
        lib/ogg_vorbis/ogg/os_types.h \
        lib/ogg_vorbis/vorbis/codec.h \
        lib/ogg_vorbis/vorbis/vorbisenc.h \
        lib/ogg_vorbis/vorbis/vorbisfile.h
    RC_FILE = polyphone.rc

    !contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += -Llib/win/32bits -lportaudio_x86 -lzlib1 -lwinmm -llibogg -llibvorbis -llibvorbisfile
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    } else {
        LIBS += -Llib/win/64bits -lportaudio_x64 -lzlib1 -lwinmm -llibogg -llibvorbis -llibvorbisfile
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
    }
}
unix:!macx {
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa jack portaudio-2.0 zlib ogg vorbis vorbisfile vorbisenc
    INCLUDEPATH += /usr/include/jack
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
    QMAKE_MAC_SDK = macosx10.9
    DEFINES += __MACOSX_CORE__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += lib/mac/Jackmp.framework/Headers \
        lib \
        lib/ogg_vorbis
    HEADERS += lib/portaudio.h \
        lib/ogg_vorbis/ogg/ogg.h \
        lib/ogg_vorbis/ogg/os_types.h \
        lib/ogg_vorbis/vorbis/codec.h \
        lib/ogg_vorbis/vorbis/vorbisenc.h \
        lib/ogg_vorbis/vorbis/vorbisfile.h
    LIBS += -L$$PWD/lib/mac -lportaudio -logg -lvorbis -F$$PWD/lib/mac/ -framework Jackmp \
        -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework AudioUnit -framework AudioToolbox -framework Cocoa -lz
    ICON = polyphone.icns
    QMAKE_INFO_PLIST = polyphone.plist
}

# Location of RtMidi
contains(DEFINES, USE_LOCAL_RTMIDI) {
    HEADERS += lib/rtmidi/RtMidi.h
    SOURCES += lib/rtmidi/RtMidi.cpp
    INCLUDEPATH += lib/rtmidi
} else {
    PKGCONFIG += rtmidi
}

# Location of Stk
contains(DEFINES, USE_LOCAL_STK) {
    HEADERS += lib/stk/Stk.h \
        lib/stk/SineWave.h \
        lib/stk/OnePole.h \
        lib/stk/Generator.h \
        lib/stk/Filter.h \
        lib/stk/Effect.h \
        lib/stk/Delay.h \
        lib/stk/Chorus.h \
        lib/stk/FreeVerb.h \
        lib/stk/DelayL.h
    SOURCES += lib/stk/Stk.cpp \
        lib/stk/SineWave.cpp \
        lib/stk/OnePole.cpp \
        lib/stk/Delay.cpp \
        lib/stk/Chorus.cpp \
        lib/stk/FreeVerb.cpp \
        lib/stk/DelayL.cpp
    INCLUDEPATH += lib/stk
} else {
    LIBS += -lstk
    INCLUDEPATH += /usr/include/stk
}

# Location of QCustomplot
contains(DEFINES, USE_LOCAL_QCUSTOMPLOT) {
    HEADERS += lib/qcustomplot/qcustomplot.h
    SOURCES += lib/qcustomplot/qcustomplot.cpp
    INCLUDEPATH += lib/qcustomplot
} else {
    LIBS += -lqcustomplot
    INCLUDEPATH += /usr/include/qcustomplot
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
    lib/sf3 \
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
    pages/graphicsviewrange.cpp \
    tools/dialog_transposition.cpp \
    sfark/sfarkextractor.cpp \
    gui_divers/translationsystem.cpp \
    pages/pageoverview.cpp \
    pages/pageoverviewsmpl.cpp \
    pages/pageoverviewinst.cpp \
    pages/pageoverviewprst.cpp \
    lib/sf3/sfont.cpp \
    sf2_core/oggconverter.cpp \
    pages/graphicslegenditem.cpp \
    pages/graphicsrectangleitem.cpp \
    pages/graphicszoomline.cpp \
    pages/graphicskey.cpp \
    pages/graphicslegenditem2.cpp \
    pages/tablepageoverview.cpp \
    gui_divers/comboboxloopmode.cpp \
    pages/tabledelegate.cpp \
    options.cpp \
    gui_divers/dialogchangelog.cpp

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
    gui_divers/dialogselection.h \
    pages/combobox.h \
    pages/tablecombobox.h \
    pages/graphicssimpletextitem.h \
    pages/graphicsviewrange.h \
    tools/dialog_transposition.h \
    sfark/sfarkextractor1.h \
    sfark/sfarkextractor.h \
    gui_divers/translationsystem.h \
    pages/pageoverview.h \
    pages/pageoverviewsmpl.h \
    pages/pageoverviewinst.h \
    pages/pageoverviewprst.h \
    lib/sf3/sfont.h \
    sf2_core/oggconverter.h \
    pages/graphicslegenditem.h \
    pages/graphicsrectangleitem.h \
    pages/graphicszoomline.h \
    pages/graphicskey.h \
    pages/graphicslegenditem2.h \
    pages/tablepageoverview.h \
    gui_divers/comboboxloopmode.h \
    pages/tabledelegate.h \
    options.h \
    gui_divers/dialogchangelog.h

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
    tools/dialog_magnetophone.ui \
    tools/dialog_celeste.ui \
    tools/dialog_visualizer.ui \
    tools/dialog_duplication.ui \
    gui_divers/dialogselection.ui \
    tools/dialog_transposition.ui \
    pages/pageoverview.ui \
    gui_divers/dialogchangelog.ui

RESOURCES += ressources.qrc \
    clavier/pianokeybd.qrc


# SfArk extraction (what a mess!)
DEFINES += __LITTLE_ENDIAN__
INCLUDEPATH += lib/sfarklib
HEADERS += lib/sfarklib/sfArkLib.h \
    lib/sfarklib/wcc.h \
    sfark/sfarkextractor2.h \
    sfark/abstractextractor.h

SPECIAL_SOURCE = sfark/sfarkextractor1.cpp
macx {
    SOURCES += sfark/sfarkextractor2.cpp \
        sfark/abstractextractor.cpp \
        lib/sfarklib/sfklZip.cpp \
        lib/sfarklib/sfklLPC.cpp \
        lib/sfarklib/sfklDiff.cpp \
        lib/sfarklib/sfklCrunch.cpp \
        lib/sfarklib/sfklCoding.cpp
} else {
    SPECIAL_SOURCE += sfark/sfarkextractor2.cpp \
        sfark/abstractextractor.cpp \
        lib/sfarklib/sfklZip.cpp \
        lib/sfarklib/sfklLPC.cpp \
        lib/sfarklib/sfklDiff.cpp \
        lib/sfarklib/sfklCrunch.cpp \
        lib/sfarklib/sfklCoding.cpp
}

ExtraCompiler.input = SPECIAL_SOURCE
ExtraCompiler.variable_out = OBJECTS
ExtraCompiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${QMAKE_EXT_OBJ}
win32 {
    ExtraCompiler.commands = $${QMAKE_CXX} -D__LITTLE_ENDIAN__ -MD -arch:IA32 -D_CRT_SECURE_NO_WARNINGS $(INCPATH) -c ${QMAKE_FILE_IN} -Fo${QMAKE_FILE_OUT}
}
unix:!macx {
    ExtraCompiler.commands = $${QMAKE_CXX} -fPIC -D__LITTLE_ENDIAN__ -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
macx {
    ExtraCompiler.commands = $${QMAKE_CXX} $(CXXFLAGS) -D__LITTLE_ENDIAN__ -mno-sse -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
QMAKE_EXTRA_COMPILERS += ExtraCompiler
