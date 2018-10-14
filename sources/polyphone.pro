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
DEFINES += USE_LOCAL_FLUIDSYNTH

# Polyphone version
DEFINES += VERSION=2.0
DEFINES += FINAL=false

# List to complete if other translations are wished
# Format: polyphone_XX.ts where XX is the language code
TRANSLATIONS = polyphone_en.ts \
    polyphone_es.ts \
    polyphone_it.ts \
    polyphone_sr.ts \
    polyphone_pt.ts \
    polyphone_cs.ts \
    polyphone_de.ts \
    polyphone_ja.ts \
    polyphone_zh.ts \
    polyphone_nl.ts \
    polyphone_ru.ts \
    polyphone_tr.ts \
    polyphone_da.ts


QT += core gui printsupport svg network
TARGET = polyphone
TEMPLATE = app

CONFIG(release, debug|release) {
    DESTDIR = RELEASE
    OBJECTS_DIR = RELEASE/.obj
    MOC_DIR = RELEASE/.moc
    RCC_DIR = RELEASE/.rcc
    UI_DIR = RELEASE/.ui
    DEFINES += QT_NO_DEBUG_OUTPUT
}
CONFIG(debug, debug|release) {
    DESTDIR = DEBUG
    OBJECTS_DIR = DEBUG/.obj
    MOC_DIR = DEBUG/.moc
    RCC_DIR = DEBUG/.rcc
    UI_DIR = DEBUG/.ui
}

win32{
    DEFINES += __WINDOWS_MM__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT USE_LOCAL_FLUIDSYNTH
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
    PKGCONFIG += alsa jack portaudio-2.0 zlib ogg vorbis vorbisfile vorbisenc glib-2.0
    INCLUDEPATH += /usr/include/jack
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
    QMAKE_MAC_SDK = macosx10.9
    DEFINES += __MACOSX_CORE__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT USE_LOCAL_FLUIDSYNTH
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

# Location of FluidSynth
contains(DEFINES, USE_LOCAL_FLUIDSYNTH) {
    INCLUDEPATH += lib/fluidsynth
    HEADERS += lib/fluidsynth/fluidsynth.h
    LIBS += -L$$PWD/lib/fluidsynth -lfluidsynth
} else {
    LIBS += -lfluidsynth
    INCLUDEPATH += /usr/include/fluidsynth
}

INCLUDEPATH += mainwindow \
    dialogs \
    context \
    context/interface \
    editor \
    editor/graphics \
    editor/overview \
    editor/tools \
    editor/widgets \
    qcustomplot \
    resources \
    core \
    core/input \
    core/output \
    core/model \
    core/types \
    clavier \
    sound_engine \
    sound_engine/elements \
    lib/sf3 \
    repository \
    repository/browser \
    repository/daily \
    repository/widgets \
    .

SOURCES	+= main.cpp \
    core/sound.cpp \
    core/pile_sf2_sl.cpp \
    core/duplicator.cpp \
    core/oggconverter.cpp \
    core/utils.cpp \
    core/input/sfz/import_sfz.cpp \
    core/input/sfark/sfarkglobal.cpp \
    core/input/sfark/sfarkfilemanager.cpp \
    core/output/sfz/conversion_sfz.cpp \
    context/contextmanager.cpp \
    context/thememanager.cpp \
    context/confmanager.cpp \
    context/recentfilemanager.cpp \
    context/keynamemanager.cpp \
    context/translationmanager.cpp \
    context/interface/editkey.cpp \
    context/interface/tablekey.cpp \
    context/audiodevice.cpp \
    context/mididevice.cpp \
    dialogs/dialog_list.cpp \
    dialogs/dialog_rename.cpp \
    dialogs/dialog_about.cpp \
    dialogs/dialog_export.cpp \
    dialogs/dialogselection.cpp \
    dialogs/dialogchangelog.cpp \
    editor/editor_old.cpp \
    editor/page.cpp \
    editor/pagetable.cpp \
    editor/envelopeditor.cpp \
    editor/graphics/graphique.cpp \
    editor/graphics/graphiquefourier.cpp \
    editor/graphics/graphicsviewrange.cpp \
    editor/graphics/graphicslegenditem.cpp \
    editor/graphics/graphicsrectangleitem.cpp \
    editor/graphics/graphicszoomline.cpp \
    editor/graphics/graphicskey.cpp \
    editor/graphics/graphicslegenditem2.cpp \
    editor/graphics/graphicsviewenvelop.cpp \
    editor/graphics/envelop.cpp \
    editor/overview/pageoverview.cpp \
    editor/overview/pageoverviewsmpl.cpp \
    editor/overview/pageoverviewinst.cpp \
    editor/overview/pageoverviewprst.cpp \
    editor/overview/tablepageoverview.cpp \
    editor/widgets/tablewidgetmod.cpp \
    editor/widgets/tablewidget.cpp \
    editor/widgets/tabledelegate.cpp \
    editor/widgets/tablecombobox.cpp \
    editor/widgets/spinboxkey.cpp \
    editor/widgets/spinboxrange.cpp \
    editor/widgets/comboboxloopmode.cpp \
    editor/tools/dialog_space.cpp \
    editor/tools/dialog_mixture.cpp \
    editor/tools/dialog_release.cpp \
    editor/tools/dialog_visualizer.cpp \
    editor/tools/dialog_duplication.cpp \
    editor/tools/dialog_transposition.cpp \
    editor/tools/duplicationtool.cpp \
    editor/tools/dialog_exportlist.cpp \
    clavier/pianoscene.cpp \
    clavier/pianokey.cpp \
    clavier/pianokeybd.cpp \
    clavier/pianokeybdcustom.cpp \
    clavier/keyboardmap.cpp \
    sound_engine/synth.cpp \
    sound_engine/voice.cpp \
    sound_engine/circularbuffer.cpp \
    sound_engine/voiceparam.cpp \
    sound_engine/soundengine.cpp \
    sound_engine/elements/calibrationsinus.cpp \
    sound_engine/elements/enveloppevol.cpp \
    sound_engine/elements/oscsinus.cpp \
    lib/sf3/sfont.cpp \
    options.cpp \
    mainwindow/widgetshowhistory.cpp \
    mainwindow/widgetshowhistorycell.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/windowmanager.cpp \
    mainwindow/coloredtabwidget.cpp \
    repository/urlreader.cpp \
    repository/urlreaderjson.cpp \
    repository/repositorymanager.cpp \
    repository/soundfontinformation.cpp \
    repository/soundfontfilter.cpp \
    repository/browser/soundfontbrowser.cpp \
    repository/browser/attributeflow.cpp \
    repository/browser/filterflow.cpp \
    repository/browser/filtertag.cpp \
    repository/browser/soundfontcellfull.cpp \
    repository/browser/filtertagcell.cpp \
    repository/daily/showsoundfonts.cpp \
    repository/daily/soundfontcell.cpp \
    repository/widgets/rotatingspinner.cpp \
    repository/widgets/fivestarswidget.cpp \
    repository/widgets/flowlayout.cpp \
    editor/editor.cpp \
    editor/pageinst.cpp \
    editor/pagesf2.cpp \
    editor/pageprst.cpp \
    editor/pagesmpl.cpp \
    core/types/idlist.cpp \
    core/soundfontmanager.cpp \
    core/actionmanager.cpp \
    core/model/soundfont.cpp \
    core/model/division.cpp \
    core/model/modulator.cpp \
    core/model/smpl.cpp \
    core/model/instprst.cpp \
    core/model/soundfonts.cpp \
    core/model/treemodel.cpp \
    core/model/treeitemfirstlevel.cpp \
    core/model/treeitemroot.cpp \
    editor/treeview.cpp \
    editor/treeitemdelegate.cpp \
    editor/widgets/backgroundwidget.cpp \
    editor/treesortfilterproxy.cpp \
    core/model/treeitem.cpp \
    editor/widgets/styledaction.cpp \
    editor/widgets/editortoolbar.cpp \
    core/actionset.cpp \
    core/action.cpp \
    editor/widgets/styledlineedit.cpp \
    core/input/abstractinput.cpp \
    core/input/inputfactory.cpp \
    core/input/sfark/inputsfark.cpp \
    core/input/not_supported/inputnotsupported.cpp \
    core/input/sfz/inputsfz.cpp \
    core/input/sf2/inputsf2.cpp \
    core/input/empty/inputempty.cpp \
    core/input/sf2/sf2header.cpp \
    core/input/sf2/sf2sdtapart.cpp \
    core/input/sf2/sf2basetypes.cpp \
    core/input/sf2/sf2pdtapart.cpp \
    core/input/sf2/sf2pdtapart_phdr.cpp \
    core/input/sf2/sf2pdtapart_inst.cpp \
    core/input/sf2/sf2pdtapart_shdr.cpp \
    core/input/sf2/sf2pdtapart_mod.cpp \
    core/input/sf2/sf2pdtapart_gen.cpp \
    core/input/sf2/sf2pdtapart_bag.cpp \
    editor/widgets/linkedtowidget.cpp \
    editor/tools/abstracttool.cpp \
    editor/tools/toolfactory.cpp \
    editor/tools/toolmenu.cpp \
    editor/tools/trim_end/tooltrimend.cpp \
    editor/tools/auto_loop/toolautoloop.cpp \
    editor/tools/abstracttooliterating.cpp \
    editor/tools/waitingtooldialog.cpp \
    editor/tools/external_command/toolexternalcommand.cpp \
    editor/tools/abstracttoolparameters.cpp \
    editor/tools/tooldialog.cpp \
    editor/tools/external_command/toolexternalcommand_parameters.cpp \
    editor/tools/external_command/toolexternalcommand_gui.cpp \
    editor/tools/trim_start/tooltrimstart.cpp \
    editor/tools/frequency_filter/toolfrequencyfilter.cpp \
    editor/tools/frequency_filter/toolfrequencyfilter_parameters.cpp \
    editor/tools/frequency_filter/toolfrequencyfilter_gui.cpp \
    editor/tools/frequency_filter/graphfilterfrequencies.cpp \
    editor/tools/change_volume/toolchangevolume.cpp \
    editor/tools/change_volume/toolchangevolume_parameters.cpp \
    editor/tools/change_volume/toolchangevolume_gui.cpp \
    editor/tools/balance_adjustment/toolbalanceadjustment.cpp \
    editor/tools/transpose_smpl/tooltransposesmpl.cpp \
    editor/tools/transpose_smpl/tooltransposesmpl_parameters.cpp \
    editor/tools/transpose_smpl/tooltransposesmpl_gui.cpp \
    context/interface/configpanel.cpp \
    sound_engine/midilistener.cpp \
    dialogs/dialogkeyboard.cpp \
    dialogs/dialogrecorder.cpp \
    editor/tools/link_sample/toollinksample.cpp \
    editor/tools/unlink_sample/toolunlinksample.cpp \
    editor/tools/change_attenuation/toolchangeattenuation.cpp \
    editor/tools/change_attenuation/toolchangeattenuation_gui.cpp \
    editor/tools/change_attenuation/toolchangeattenuation_parameters.cpp \
    editor/tools/global_settings/toolglobalsettings.cpp \
    editor/tools/global_settings/toolglobalsettings_gui.cpp \
    editor/tools/global_settings/toolglobalsettings_parameters.cpp \
    editor/tools/global_settings/graphparamglobal.cpp \
    core/types/eltid.cpp \
    core/types/complex.cpp \
    core/types/attribute.cpp \
    editor/tools/celeste_tuning/toolcelestetuning.cpp \
    editor/tools/celeste_tuning/toolcelestetuning_gui.cpp \
    editor/tools/celeste_tuning/toolcelestetuning_parameters.cpp

HEADERS  += \
    core/sound.h \
    core/duplicator.h \
    core/oggconverter.h \
    core/utils.h \
    core/input/sfz/import_sfz.h \
    core/input/sfark/sfarkglobal.h \
    core/input/sfark/sfarkfilemanager.h \
    core/input/sfark/sfarkextractor1.h \
    core/output/sfz/conversion_sfz.h \
    context/contextmanager.h \
    context/thememanager.h \
    context/confmanager.h \
    context/recentfilemanager.h \
    context/keynamemanager.h \
    context/translationmanager.h \
    context/interface/editkey.h \
    context/interface/tablekey.h \
    context/mididevice.h \
    context/audiodevice.h \
    dialogs/dialog_list.h \
    dialogs/dialog_rename.h \
    dialogs/dialog_about.h \
    dialogs/dialog_export.h \
    dialogs/dialogselection.h \
    dialogs/dialogchangelog.h \
    dialogs/modalprogressdialog.h \
    editor/editor_old.h \
    editor/page.h \
    editor/pagetable.h \
    editor/envelopeditor.h \
    editor/graphics/graphique.h \
    editor/graphics/graphiquefourier.h \
    editor/graphics/graphicssimpletextitem.h \
    editor/graphics/graphicsviewrange.h \
    editor/graphics/graphicslegenditem.h \
    editor/graphics/graphicsrectangleitem.h \
    editor/graphics/graphicszoomline.h \
    editor/graphics/graphicskey.h \
    editor/graphics/graphicslegenditem2.h \
    editor/graphics/graphicsviewenvelop.h \
    editor/graphics/envelop.h \
    editor/overview/pageoverview.h \
    editor/overview/pageoverviewsmpl.h \
    editor/overview/pageoverviewinst.h \
    editor/overview/pageoverviewprst.h \
    editor/overview/tablepageoverview.h \
    editor/widgets/tablewidgetmod.h \
    editor/widgets/tablewidget.h \
    editor/widgets/combobox.h \
    editor/widgets/tablecombobox.h \
    editor/widgets/tabledelegate.h \
    editor/widgets/spinboxkey.h \
    editor/widgets/spinboxrange.h \
    editor/widgets/comboboxloopmode.h \
    editor/tools/dialog_space.h \
    editor/tools/dialog_mixture.h \
    editor/tools/dialog_release.h \
    editor/tools/dialog_visualizer.h \
    editor/tools/dialog_duplication.h \
    editor/tools/dialog_transposition.h \
    editor/tools/duplicationtool.h \
    editor/tools/dialog_exportlist.h \
    clavier/pianoscene.h \
    clavier/pianokey.h \
    clavier/keyboardmap.h \
    clavier/pianokeybd.h \
    clavier/pianokeybdcustom.h \
    clavier/keyboardmap.h \
    sound_engine/synth.h \
    sound_engine/voice.h \
    sound_engine/circularbuffer.h \
    sound_engine/voiceparam.h \
    sound_engine/soundengine.h \
    sound_engine/elements/calibrationsinus.h \
    sound_engine/elements/enveloppevol.h \
    sound_engine/elements/oscsinus.h \
    macapplication.h \
    lib/sf3/sfont.h \
    options.h \
    mainwindow/widgetshowhistory.h \
    mainwindow/widgetshowhistorycell.h \
    mainwindow/mainwindow.h \
    mainwindow/windowmanager.h \
    mainwindow/coloredtabwidget.h \
    repository/urlreader.h \
    repository/urlreaderjson.h \
    repository/repositorymanager.h \
    repository/soundfontinformation.h \
    repository/soundfontfilter.h \
    repository/browser/soundfontbrowser.h \
    repository/browser/attributeflow.h \
    repository/browser/filterflow.h \
    repository/browser/filtertag.h \
    repository/browser/soundfontcellfull.h \
    repository/browser/filtertagcell.h \
    repository/daily/showsoundfonts.h \
    repository/daily/soundfontcell.h \
    repository/widgets/rotatingspinner.h \
    repository/widgets/flowlayout.h \
    repository/widgets/fivestarswidget.h \
    editor/editor.h \
    editor/pageinst.h \
    editor/pageprst.h \
    editor/pagesf2.h \
    editor/pagesmpl.h \
    core/types/idlist.h \
    core/soundfontmanager.h \
    core/actionmanager.h \
    core/model/soundfont.h \
    core/model/division.h \
    core/model/modulator.h \
    core/model/smpl.h \
    core/model/instprst.h \
    core/model/soundfonts.h \
    core/model/treeitem.h \
    core/model/treemodel.h \
    core/model/treeitemfirstlevel.h \
    core/model/treeitemroot.h \
    editor/treeview.h \
    editor/treeitemdelegate.h \
    editor/widgets/backgroundwidget.h \
    editor/treesortfilterproxy.h \
    editor/widgets/styledaction.h \
    editor/widgets/editortoolbar.h \
    core/actionset.h \
    core/action.h \
    editor/widgets/styledlineedit.h \
    core/input/abstractinput.h \
    core/input/inputfactory.h \
    core/input/sfark/inputsfark.h \
    core/input/not_supported/inputnotsupported.h \
    core/input/sfz/inputsfz.h \
    core/input/sf2/inputsf2.h \
    core/input/empty/inputempty.h \
    core/input/sf2/sf2header.h \
    core/input/sf2/sf2sdtapart.h \
    core/input/sf2/sf2basetypes.h \
    core/input/sf2/sf2pdtapart.h \
    core/input/sf2/sf2pdtapart_phdr.h \
    core/input/sf2/sf2pdtapart_inst.h \
    core/input/sf2/sf2pdtapart_shdr.h \
    core/input/sf2/sf2pdtapart_mod.h \
    core/input/sf2/sf2pdtapart_gen.h \
    core/input/sf2/sf2pdtapart_bag.h \
    repository/widgets/elidedlabel.h \
    mainwindow/tabbar.h \
    editor/widgets/linkedtowidget.h \
    editor/tools/abstracttool.h \
    editor/tools/toolfactory.h \
    editor/tools/toolmenu.h \
    editor/tools/trim_end/tooltrimend.h \
    editor/tools/auto_loop/toolautoloop.h \
    editor/tools/abstracttooliterating.h \
    editor/tools/waitingtooldialog.h \
    editor/tools/external_command/toolexternalcommand.h \
    editor/tools/abstracttoolparameters.h \
    editor/tools/tooldialog.h \
    editor/tools/abstracttoolgui.h \
    editor/tools/external_command/toolexternalcommand_parameters.h \
    editor/tools/external_command/toolexternalcommand_gui.h \
    editor/tools/trim_start/tooltrimstart.h \
    editor/tools/frequency_filter/toolfrequencyfilter.h \
    editor/tools/frequency_filter/toolfrequencyfilter_parameters.h \
    editor/tools/frequency_filter/toolfrequencyfilter_gui.h \
    editor/tools/frequency_filter/graphfilterfrequencies.h \
    editor/tools/change_volume/toolchangevolume.h \
    editor/tools/change_volume/toolchangevolume_parameters.h \
    editor/tools/change_volume/toolchangevolume_gui.h \
    editor/tools/balance_adjustment/toolbalanceadjustment.h \
    editor/tools/transpose_smpl/tooltransposesmpl.h \
    editor/tools/transpose_smpl/tooltransposesmpl_parameters.h \
    editor/tools/transpose_smpl/tooltransposesmpl_gui.h \
    context/interface/configpanel.h \
    sound_engine/midilistener.h \
    dialogs/dialogkeyboard.h \
    dialogs/dialogrecorder.h \
    context/controllerevent.h \
    context/noteevent.h \
    editor/tools/link_sample/toollinksample.h \
    editor/tools/unlink_sample/toolunlinksample.h \
    editor/tools/change_attenuation/toolchangeattenuation.h \
    editor/tools/change_attenuation/toolchangeattenuation_gui.h \
    editor/tools/change_attenuation/toolchangeattenuation_parameters.h \
    editor/tools/global_settings/toolglobalsettings.h \
    editor/tools/global_settings/toolglobalsettings_gui.h \
    editor/tools/global_settings/toolglobalsettings_parameters.h \
    editor/tools/global_settings/graphparamglobal.h \
    core/types/eltid.h \
    core/types/complex.h \
    core/types/attribute.h \
    core/types/basetypes.h \
    editor/tools/celeste_tuning/toolcelestetuning.h \
    editor/tools/celeste_tuning/toolcelestetuning_gui.h \
    editor/tools/celeste_tuning/toolcelestetuning_parameters.h

FORMS    += \
    dialogs/dialog_list.ui \
    dialogs/dialog_rename.ui \
    dialogs/dialog_about.ui \
    dialogs/dialog_export.ui \
    dialogs/dialogselection.ui \
    dialogs/dialogchangelog.ui \
    editor/editor_old.ui \
    editor/envelopeditor.ui \
    editor/overview/pageoverview.ui \
    editor/tools/dialog_space.ui \
    editor/tools/dialog_mixture.ui \
    editor/tools/dialog_release.ui \
    editor/tools/dialog_visualizer.ui \
    editor/tools/dialog_duplication.ui \
    editor/tools/dialog_transposition.ui \
    editor/tools/dialog_exportlist.ui \
    mainwindow/widgetshowhistory.ui \
    mainwindow/widgetshowhistorycell.ui \
    mainwindow/mainwindow.ui \
    repository/browser/soundfontbrowser.ui \
    repository/browser/filtertag.ui \
    repository/browser/soundfontcellfull.ui \
    repository/browser/filtertagcell.ui \
    repository/daily/showsoundfonts.ui \
    repository/daily/soundfontcell.ui \
    repository/widgets/fivestarswidget.ui \
    editor/editor.ui \
    editor/pageinst.ui \
    editor/pageprst.ui \
    editor/pagesmpl.ui \
    editor/pagesf2.ui \
    editor/tools/waitingtooldialog.ui \
    editor/tools/external_command/toolexternalcommand_gui.ui \
    editor/tools/frequency_filter/toolfrequencyfilter_gui.ui \
    editor/tools/change_volume/toolchangevolume_gui.ui \
    editor/tools/transpose_smpl/tooltransposesmpl_gui.ui \
    context/interface/configpanel.ui \
    dialogs/dialogkeyboard.ui \
    dialogs/dialogrecorder.ui \
    editor/tools/change_attenuation/toolchangeattenuation_gui.ui \
    editor/tools/global_settings/toolglobalsettings_gui.ui \
    editor/tools/celeste_tuning/toolcelestetuning_gui.ui

RESOURCES += resources.qrc


# SfArk extraction (what a mess!)
DEFINES += __LITTLE_ENDIAN__
INCLUDEPATH += lib/sfarklib
HEADERS += lib/sfarklib/sfArkLib.h \
    lib/sfarklib/wcc.h \
    core/input/sfark/sfarkextractor2.h \
    core/input/sfark/abstractextractor.h

SPECIAL_SOURCE = core/input/sfark/sfarkextractor1.cpp
macx {
    SOURCES += core/input/sfark/sfarkextractor2.cpp \
        core/input/sfark/abstractextractor.cpp \
        lib/sfarklib/sfklZip.cpp \
        lib/sfarklib/sfklLPC.cpp \
        lib/sfarklib/sfklDiff.cpp \
        lib/sfarklib/sfklCrunch.cpp \
        lib/sfarklib/sfklCoding.cpp
} else {
    SPECIAL_SOURCE += core/input/sfark/sfarkextractor2.cpp \
        core/input/sfark/abstractextractor.cpp \
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
equals(QMAKE_CXX, g++) {
    ExtraCompiler.commands = $${QMAKE_CXX} -fPIC -D__LITTLE_ENDIAN__ -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
equals(QMAKE_CXX, clang++) {
    ExtraCompiler.commands = $${QMAKE_CXX} $(CXXFLAGS) -D__LITTLE_ENDIAN__ -mno-sse -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
QMAKE_EXTRA_COMPILERS += ExtraCompiler
