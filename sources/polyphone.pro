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
DEFINES += SOFT_VERSION=\\\"2.0.1\\\"
DEFINES += IDENTIFIER=\\\"\\\"

OBJECTS_DIR=generated_files
MOC_DIR=generated_files
UI_DIR=generated_files
RCC_DIR=generated_files

# List to be completed if other translations are desired
# Format: polyphone_XX.ts, XX being the language code
TRANSLATIONS = polyphone_fr.ts \
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


QT += core gui printsupport svg network #testlib
TARGET = polyphone
TEMPLATE = app

win32{
    DEFINES += __WINDOWS_MM__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += lib \
        lib/ogg_vorbis \
        ../lib_windows/include
    RC_FILE = polyphone.rc

    !contains(QMAKE_TARGET.arch, x86_64) {
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
        LIBS += -L$$PWD/../lib_windows/32bits -lportaudio_x86
        DESTDIR = $$PWD/../lib_windows/32bits
    } else {
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
        LIBS += -L$$PWD/../lib_windows/64bits -lportaudio_x64
        DESTDIR = $$PWD/../lib_windows/64bits
    }
    LIBS += -lzlib1 -lwinmm -llibogg -llibvorbis -llibvorbisfile -lcrypto
}
unix:!macx {
    DEFINES += __LINUX_ALSASEQ__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa jack portaudio-2.0 zlib ogg vorbis vorbisfile vorbisenc glib-2.0 openssl
    INCLUDEPATH += /usr/include/jack
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    TARGET.path = $$PREFIX/
    DESTDIR=bin
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_MAC_SDK = macosx10.14
    DEFINES += __MACOSX_CORE__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += ../lib_mac/Jackmp.framework/Headers \
        lib \
        lib/ogg_vorbis \
        ../lib_mac/include
    LIBS += -L$$PWD/../lib_mac -lportaudio -logg -lvorbis -lssl -lcrypto -F$$PWD/../lib_mac/ -framework Jackmp \
        -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework AudioUnit -framework AudioToolbox -framework Cocoa -lz
    ICON = polyphone.icns
    QMAKE_INFO_PLIST = polyphone.plist
    DESTDIR = $$PWD/../lib_mac
}
DEFINES += SFTOOLS_NOXML

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

INCLUDEPATH += mainwindow \
    dialogs \
    context \
    context/interface \
    editor \
    editor/graphics \
    editor/overview \
    editor/tools \
    editor/widgets \
    editor/tree \
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
    repository/user \
    repository/soundfont \
    .

SOURCES	+= main.cpp \
    core/sound.cpp \
    core/duplicator.cpp \
    core/utils.cpp \
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
    context/audiodevice.cpp \
    context/mididevice.cpp \
    dialogs/dialog_list.cpp \
    dialogs/dialog_rename.cpp \
    dialogs/dialog_about.cpp \
    dialogs/dialogselection.cpp \
    dialogs/dialogchangelog.cpp \
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
    editor/tools/division_duplication/duplicationtool.cpp \
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
    editor/tree/treeview.cpp \
    editor/tree/treeitemdelegate.cpp \
    editor/widgets/backgroundwidget.cpp \
    editor/tree/treesortfilterproxy.cpp \
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
    editor/tools/celeste_tuning/toolcelestetuning_parameters.cpp \
    editor/tools/auto_distribution/toolautodistribution.cpp \
    editor/tools/clean_unused_elements/toolcleanunused.cpp \
    editor/tools/division_duplication/tooldivisionduplication.cpp \
    editor/tools/division_duplication/tooldivisionduplication_gui.cpp \
    editor/tools/division_duplication/tooldivisionduplication_parameters.cpp \
    editor/tools/frequency_peaks/toolfrequencypeaks.cpp \
    editor/tools/mixture_creation/toolmixturecreation.cpp \
    editor/tools/mixture_creation/toolmixturecreation_gui.cpp \
    editor/tools/mixture_creation/toolmixturecreation_parameters.cpp \
    editor/tools/monitor/toolmonitor.cpp \
    editor/tools/monitor/toolmonitor_gui.cpp \
    editor/tools/monitor/toolmonitor_parameters.cpp \
    editor/tools/preset_list/toolpresetlist.cpp \
    editor/tools/preset_list/toolpresetlist_gui.cpp \
    editor/tools/preset_list/toolpresetlist_parameters.cpp \
    editor/tools/remove_mods/toolremovemods.cpp \
    editor/tools/sound_spatialization/toolsoundspatialization.cpp \
    editor/tools/sound_spatialization/toolsoundspatialization_gui.cpp \
    editor/tools/sound_spatialization/toolsoundspatialization_parameters.cpp \
    editor/tools/transpose/tooltranspose.cpp \
    editor/tools/transpose/tooltranspose_gui.cpp \
    editor/tools/transpose/tooltranspose_parameters.cpp \
    editor/tree/treeviewmenu.cpp \
    core/sampleloader.cpp \
    editor/tools/frequency_peaks/toolfrequencypeaks_gui.cpp \
    editor/tools/frequency_peaks/toolfrequencypeaks_parameters.cpp \
    editor/tools/mixture_creation/runnablesamplecreator.cpp \
    editor/tools/monitor/graphlegend.cpp \
    editor/tools/monitor/graphvisualizer.cpp \
    editor/tools/sound_spatialization/graphspace.cpp \
    editor/tools/release/toolrelease.cpp \
    editor/tools/release/toolrelease_gui.cpp \
    editor/tools/release/toolrelease_parameters.cpp \
    editor/tools/chords/runnablechordcreator.cpp \
    editor/tools/chords/toolchords.cpp \
    editor/tools/chords/toolchords_gui.cpp \
    editor/tools/chords/toolchords_parameters.cpp \
    mainwindow/mainmenu.cpp \
    editor/tools/sample_export/toolsampleexport.cpp \
    editor/tools/soundfont_export/toolsoundfontexport.cpp \
    editor/tools/soundfont_export/toolsoundfontexport_gui.cpp \
    editor/tools/soundfont_export/toolsoundfontexport_parameters.cpp \
    editor/tools/abstracttoolonestep.cpp \
    core/output/abstractoutput.cpp \
    core/output/outputfactory.cpp \
    core/output/empty/outputdummy.cpp \
    core/output/sf2/outputsf2.cpp \
    core/output/sfz/outputsfz.cpp \
    core/output/not_supported/outputnotsupported.cpp \
    core/output/sfz/sfzparamlist.cpp \
    core/output/sf2/sf2indexconverter.cpp \
    core/output/sf3/outputsf3.cpp \
    core/input/sf3/inputsf3.cpp \
    core/input/sfz/sfzparameter.cpp \
    core/input/sfz/sfzparametergroup.cpp \
    core/input/sfz/sfzparametergroupassembly.cpp \
    dialogs/dialogcreateelements.cpp \
    editor/widgets/tableheaderview.cpp \
    context/interface/configtoc.cpp \
    context/interface/configtocbutton.cpp \
    context/interface/configsectiongeneral.cpp \
    context/interface/configsectioninterface.cpp \
    context/interface/configsectionsound.cpp \
    context/interface/configsectionkeyboard.cpp \
    context/interface/configsectionrepository.cpp \
    repository/usermanager.cpp \
    mainwindow/toprightwidget.cpp \
    repository/user/userarea.cpp \
    repository/soundfont/soundfontviewer.cpp \
    repository/widgets/elidedpushbutton.cpp \
    repository/soundfont/soundfontdescription.cpp \
    repository/soundfont/soundfontdownload.cpp \
    repository/soundfont/soundfontcomment.cpp \
    repository/soundfontdetails.cpp \
    repository/detailsmanager.cpp \
    repository/soundfont/soundfontcommentdata.cpp \
    repository/soundfont/soundfontdescriptiondata.cpp \
    repository/soundfont/soundfontdownloaddata.cpp \
    repository/urlreaderimage.cpp \
    repository/soundfont/soundfontdownloadcell.cpp \
    repository/soundfont/soundfontcommentcell.cpp \
    repository/downloadmanager.cpp \
    repository/widgets/downloadprogressbutton.cpp \
    repository/widgets/downloadprogresscell.cpp \
    editor/overview/sortedtablewidgetitem.cpp \
    repository/widgets/elidedlabel.cpp

HEADERS  += \
    core/sound.h \
    core/duplicator.h \
    core/utils.h \
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
    context/mididevice.h \
    context/audiodevice.h \
    dialogs/dialog_list.h \
    dialogs/dialog_rename.h \
    dialogs/dialog_about.h \
    dialogs/dialogselection.h \
    dialogs/dialogchangelog.h \
    dialogs/modalprogressdialog.h \
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
    editor/tools/division_duplication/duplicationtool.h \
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
    editor/tree/treeview.h \
    editor/tree/treeitemdelegate.h \
    editor/widgets/backgroundwidget.h \
    editor/tree/treesortfilterproxy.h \
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
    editor/tools/celeste_tuning/toolcelestetuning_parameters.h \
    editor/tools/auto_distribution/toolautodistribution.h \
    editor/tools/clean_unused_elements/toolcleanunused.h \
    editor/tools/division_duplication/tooldivisionduplication.h \
    editor/tools/division_duplication/tooldivisionduplication_gui.h \
    editor/tools/division_duplication/tooldivisionduplication_parameters.h \
    editor/tools/frequency_peaks/toolfrequencypeaks.h \
    editor/tools/mixture_creation/toolmixturecreation.h \
    editor/tools/mixture_creation/toolmixturecreation_gui.h \
    editor/tools/mixture_creation/toolmixturecreation_parameters.h \
    editor/tools/monitor/toolmonitor.h \
    editor/tools/monitor/toolmonitor_gui.h \
    editor/tools/monitor/toolmonitor_parameters.h \
    editor/tools/preset_list/toolpresetlist.h \
    editor/tools/preset_list/toolpresetlist_gui.h \
    editor/tools/preset_list/toolpresetlist_parameters.h \
    editor/tools/remove_mods/toolremovemods.h \
    editor/tools/sound_spatialization/toolsoundspatialization.h \
    editor/tools/sound_spatialization/toolsoundspatialization_gui.h \
    editor/tools/sound_spatialization/toolsoundspatialization_parameters.h \
    editor/tools/transpose/tooltranspose.h \
    editor/tools/transpose/tooltranspose_gui.h \
    editor/tools/transpose/tooltranspose_parameters.h \
    editor/tree/treeviewmenu.h \
    core/sampleloader.h \
    editor/tools/frequency_peaks/toolfrequencypeaks_gui.h \
    editor/tools/frequency_peaks/toolfrequencypeaks_parameters.h \
    editor/tools/mixture_creation/runnablesamplecreator.h \
    editor/tools/monitor/graphlegend.h \
    editor/tools/monitor/graphvisualizer.h \
    editor/tools/sound_spatialization/graphspace.h \
    editor/tools/release/toolrelease.h \
    editor/tools/release/toolrelease_gui.h \
    editor/tools/release/toolrelease_parameters.h \
    editor/tools/chords/runnablechordcreator.h \
    editor/tools/chords/toolchords.h \
    editor/tools/chords/toolchords_gui.h \
    editor/tools/chords/toolchords_parameters.h \
    mainwindow/mainmenu.h \
    editor/tools/sample_export/toolsampleexport.h \
    editor/tools/soundfont_export/toolsoundfontexport.h \
    editor/tools/soundfont_export/toolsoundfontexport_gui.h \
    editor/tools/soundfont_export/toolsoundfontexport_parameters.h \
    editor/tools/abstracttoolonestep.h \
    core/output/abstractoutput.h \
    core/output/outputfactory.h \
    core/output/empty/outputdummy.h \
    core/output/sf2/outputsf2.h \
    core/output/sfz/outputsfz.h \
    core/output/not_supported/outputnotsupported.h \
    core/output/sfz/sfzparamlist.h \
    core/output/sf2/sf2indexconverter.h \
    core/output/sf3/outputsf3.h \
    core/input/sf3/inputsf3.h \
    core/input/sfz/sfzparameter.h \
    core/input/sfz/sfzparametergroup.h \
    core/input/sfz/sfzparametergroupassembly.h \
    dialogs/dialogcreateelements.h \
    editor/widgets/tableheaderview.h \
    context/interface/configtoc.h \
    context/interface/configtocbutton.h \
    context/interface/configsectiongeneral.h \
    context/interface/configsectioninterface.h \
    context/interface/configsectionsound.h \
    context/interface/configsectionkeyboard.h \
    context/interface/configsectionrepository.h \
    repository/usermanager.h \
    mainwindow/toprightwidget.h \
    repository/user/userarea.h \
    repository/soundfont/soundfontviewer.h \
    repository/widgets/elidedpushbutton.h \
    repository/soundfont/soundfontdescription.h \
    repository/soundfont/soundfontdownload.h \
    repository/soundfont/soundfontcomment.h \
    repository/soundfontdetails.h \
    repository/detailsmanager.h \
    repository/soundfont/soundfontcommentdata.h \
    repository/soundfont/soundfontdescriptiondata.h \
    repository/soundfont/soundfontdownloaddata.h \
    repository/urlreaderimage.h \
    repository/soundfont/soundfontdownloadcell.h \
    repository/soundfont/soundfontcommentcell.h \
    repository/downloadmanager.h \
    repository/widgets/downloadprogressbutton.h \
    repository/widgets/downloadprogresscell.h \
    core/types/indexedelementlist.h \
    editor/overview/sortedtablewidgetitem.h

FORMS    += \
    dialogs/dialog_list.ui \
    dialogs/dialog_rename.ui \
    dialogs/dialog_about.ui \
    dialogs/dialogselection.ui \
    dialogs/dialogchangelog.ui \
    editor/envelopeditor.ui \
    editor/overview/pageoverview.ui \
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
    editor/tools/celeste_tuning/toolcelestetuning_gui.ui \
    editor/tools/division_duplication/tooldivisionduplication_gui.ui \
    editor/tools/mixture_creation/toolmixturecreation_gui.ui \
    editor/tools/monitor/toolmonitor_gui.ui \
    editor/tools/preset_list/toolpresetlist_gui.ui \
    editor/tools/sound_spatialization/toolsoundspatialization_gui.ui \
    editor/tools/transpose/tooltranspose_gui.ui \
    editor/tools/frequency_peaks/toolfrequencypeaks_gui.ui \
    editor/tools/release/toolrelease_gui.ui \
    editor/tools/chords/toolchords_gui.ui \
    editor/tools/soundfont_export/toolsoundfontexport_gui.ui \
    dialogs/dialogcreateelements.ui \
    context/interface/configtoc.ui \
    context/interface/configsectiongeneral.ui \
    context/interface/configsectioninterface.ui \
    context/interface/configsectionsound.ui \
    context/interface/configsectionkeyboard.ui \
    context/interface/configsectionrepository.ui \
    mainwindow/toprightwidget.ui \
    repository/user/userarea.ui \
    repository/soundfont/soundfontviewer.ui \
    repository/soundfont/soundfontdescription.ui \
    repository/soundfont/soundfontdownload.ui \
    repository/soundfont/soundfontcomment.ui \
    repository/soundfont/soundfontdownloadcell.ui \
    repository/soundfont/soundfontcommentcell.ui \
    repository/widgets/downloadprogresscell.ui

RESOURCES += resources.qrc


# SfArk extraction (what a mess!)
DEFINES += __LITTLE_ENDIAN__
INCLUDEPATH += lib/sfarklib
HEADERS += lib/sfarklib/sfArkLib.h \
    lib/sfarklib/wcc.h \
    core/input/sfark/sfarkextractor2.h \
    core/input/sfark/abstractextractor.h

SPECIAL_SOURCES = core/input/sfark/sfarkextractor1.cpp
macx {
    SOURCES += core/input/sfark/sfarkextractor2.cpp \
        lib/sfarklib/sfklZip.cpp \
        lib/sfarklib/sfklLPC.cpp \
        lib/sfarklib/sfklDiff.cpp \
        lib/sfarklib/sfklCrunch.cpp \
        lib/sfarklib/sfklCoding.cpp
} else {
    SPECIAL_SOURCES += core/input/sfark/sfarkextractor2.cpp \
        lib/sfarklib/sfklZip.cpp \
        lib/sfarklib/sfklLPC.cpp \
        lib/sfarklib/sfklDiff.cpp \
        lib/sfarklib/sfklCrunch.cpp \
        lib/sfarklib/sfklCoding.cpp
}

ExtraCompiler.input = SPECIAL_SOURCES
ExtraCompiler.variable_out = OBJECTS
ExtraCompiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${QMAKE_EXT_OBJ}
win32 {
    ExtraCompiler.commands = $${QMAKE_CXX} -D__LITTLE_ENDIAN__ -MD -arch:IA32 -D_CRT_SECURE_NO_WARNINGS $(INCPATH) -c ${QMAKE_FILE_IN} -Fo${QMAKE_FILE_OUT}
}
equals(QMAKE_CXX, g++) {
    ExtraCompiler.commands = $${QMAKE_CXX} -fPIC -D__LITTLE_ENDIAN__ -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
macx {
    ExtraCompiler.commands = $${QMAKE_CXX} $(CXXFLAGS) -D__LITTLE_ENDIAN__ -mno-sse -mfpmath=387 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}
QMAKE_EXTRA_COMPILERS += ExtraCompiler
