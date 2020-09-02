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
DEFINES += USE_LOCAL_SFARKLIB

# Uncomment this line to use wolfssl instead of openssl (for a license issue)
#DEFINES += USE_WOLFSSL

# Polyphone version
DEFINES += SOFT_VERSION=\\\"2.2.0\\\"
DEFINES += IDENTIFIER=\\\"\\\"

# List to be completed if other translations are desired
# Format: polyphone_XX.ts, XX being the language code
TRANSLATIONS = polyphone_en.ts \
    polyphone_fr.ts \
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
    polyphone_da.ts \
    polyphone_gr.ts \
    polyphone_ko.ts
CONFIG += lrelease embed_translations
QMAKE_LRELEASE_FLAGS = -nounfinished -removeidentical


QT += core gui printsupport svg network #testlib
TARGET = polyphone
TEMPLATE = app

include(lib/qtsingleapplication/qtsingleapplication.pri)

win32 {
    DEFINES += __WINDOWS_MM__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += ../lib_windows/include
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
    LIBS += -lzlib1 -lwinmm -llibogg -llibvorbis -llibvorbisfile -lcrypto -llibFLAC
}
unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11 -ffloat-store
    DEFINES += __LINUX_ALSASEQ__ __UNIX_JACK__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa jack portaudio-2.0 zlib ogg flac vorbis vorbisfile vorbisenc glib-2.0
    contains(DEFINES, USE_WOLFSSL) {
        PKGCONFIG += wolfssl
    } else {
        PKGCONFIG += openssl
    }
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    DESTDIR=bin

    # Install target

    target.path = $$PREFIX/bin
    install_image_svg.path = $$PREFIX/share/icons/hicolor/scalable/apps
    install_image_svg.files = contrib/polyphone.svg
    install_image_png.path = $$PREFIX/share/icons/hicolor/512x512/apps
    install_image_png.files = resources/polyphone.png
    install_image_mimetype.path = $$PREFIX/share/icons/hicolor/scalable/mimetypes
    install_image_mimetype.files = contrib/audio-x-soundfont.svg
    install_desktop.path = $$PREFIX/share/applications
    install_desktop.files = contrib/com.polyphone_soundfonts.polyphone.desktop
    install_appdata.path = $$PREFIX/share/metainfo
    install_appdata.files = contrib/com.polyphone_soundfonts.polyphone.metainfo.xml
    install_mime.path = $$PREFIX/share/mime/packages
    install_mime.files = contrib/polyphone.xml
    install_man.path = $$PREFIX/share
    install_man.files = contrib/man
    install_doc.path = $$PREFIX/share/doc/polyphone
    install_doc.files = ../README.md changelog

    INSTALLS += target install_image_svg install_image_png install_image_mimetype \
                install_desktop install_appdata install_mime install_man install_doc
}
macx {
    QMAKE_CXXFLAGS += -std=c++11 -ffloat-store
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_MAC_SDK = macosx10.14
    DEFINES += __MACOSX_CORE__ USE_LOCAL_RTMIDI USE_LOCAL_STK USE_LOCAL_QCUSTOMPLOT
    INCLUDEPATH += ../lib_mac/Jackmp.framework/Headers \
        ../lib_mac/include
    LIBS += -L$$PWD/../lib_mac -lportaudio -logg -lFLAC -lvorbis -lssl -lcrypto -F$$PWD/../lib_mac/ -framework Jackmp \
        -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework AudioUnit -framework AudioToolbox -framework Cocoa -lz
    ICON = polyphone.icns
    QMAKE_INFO_PLIST = polyphone.plist
    DESTDIR = $$PWD/../lib_mac
}
DEFINES += SFTOOLS_NOXML

# Location of RtMidi
contains(DEFINES, USE_LOCAL_RTMIDI) {
    INCLUDEPATH += lib/_option_rtmidi
    HEADERS += lib/_option_rtmidi/rtmidi/RtMidi.h
    SOURCES += lib/_option_rtmidi/rtmidi/RtMidi.cpp
} else {
    PKGCONFIG += rtmidi
}

# Location of Stk
contains(DEFINES, USE_LOCAL_STK) {
    INCLUDEPATH += lib/_option_stk
    HEADERS += lib/_option_stk/stk/Stk.h \
        lib/_option_stk/stk/SineWave.h \
        lib/_option_stk/stk/OnePole.h \
        lib/_option_stk/stk/Generator.h \
        lib/_option_stk/stk/Filter.h \
        lib/_option_stk/stk/Effect.h \
        lib/_option_stk/stk/Delay.h \
        lib/_option_stk/stk/Chorus.h \
        lib/_option_stk/stk/FreeVerb.h \
        lib/_option_stk/stk/DelayL.h \
        lib/_option_stk/stk/Iir.h
    SOURCES += lib/_option_stk/stk/Stk.cpp \
        lib/_option_stk/stk/SineWave.cpp \
        lib/_option_stk/stk/OnePole.cpp \
        lib/_option_stk/stk/Delay.cpp \
        lib/_option_stk/stk/Chorus.cpp \
        lib/_option_stk/stk/FreeVerb.cpp \
        lib/_option_stk/stk/DelayL.cpp \
        lib/_option_stk/stk/Iir.cpp
} else {
    LIBS += -lstk
}

# Location of QCustomplot
contains(DEFINES, USE_LOCAL_QCUSTOMPLOT) {
    INCLUDEPATH += lib/_option_qcustomplot
    HEADERS += lib/_option_qcustomplot/qcustomplot.h
    SOURCES += lib/_option_qcustomplot/qcustomplot.cpp
} else {
    LIBS += -lqcustomplot
}

# Location of sfArk
HEADERS += \
    core/input/sfark/sfarkextractor2.h \
    core/input/sfark/abstractextractor.h
SOURCES += core/input/sfark/sfarkextractor1.cpp \
    core/input/sfark/sfarkextractor2.cpp
contains(DEFINES, USE_LOCAL_SFARKLIB) {
    DEFINES += __LITTLE_ENDIAN__
    INCLUDEPATH += lib/_option_sfarklib
    HEADERS += lib/_option_sfarklib/sfArkLib.h
    SOURCES += lib/_option_sfarklib/sfklZip.cpp \
        lib/_option_sfarklib/sfklLPC.cpp \
        lib/_option_sfarklib/sfklDiff.cpp \
        lib/_option_sfarklib/sfklCrunch.cpp \
        lib/_option_sfarklib/sfklCoding.cpp
} else {
    LIBS += -lsfark
}

INCLUDEPATH += lib \
    mainwindow \
    dialogs \
    context \
    context/interface \
    editor \
    editor/graphics \
    editor/modulator \
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
    core/sample \
    core/types \
    clavier \
    sound_engine \
    sound_engine/elements \
    repository \
    repository/browser \
    repository/daily \
    repository/widgets \
    repository/user \
    repository/soundfont \
    repository/soundfont/viewer \
    repository/soundfont/editor \
    .

SOURCES	+= main.cpp \
    core/input/grandorgue/grandorguedatathrough.cpp \
    core/input/grandorgue/grandorgueranklink.cpp \
    core/solomanager.cpp \
    core/input/abstractinputparser.cpp \
    core/input/empty/inputparserempty.cpp \
    core/input/grandorgue/grandorguepipe.cpp \
    core/input/grandorgue/grandorguerank.cpp \
    core/input/grandorgue/grandorguestop.cpp \
    core/input/grandorgue/inputgrandorgue.cpp \
    core/input/grandorgue/inputparsergrandorgue.cpp \
    core/input/not_supported/inputparsernotsupported.cpp \
    core/input/sf2/inputparsersf2.cpp \
    core/input/sf2/inputsf2.cpp \
    core/input/sf3/inputparsersf3.cpp \
    core/input/sf3/inputsf3.cpp \
    core/input/sfark/inputparsersfark.cpp \
    core/input/sfark/inputsfark.cpp \
    core/input/sfz/inputparsersfz.cpp \
    core/input/sfz/inputsfz.cpp \
    core/sample/samplereaderfactory.cpp \
    core/sample/samplereaderflac.cpp \
    core/sample/samplereadersf2.cpp \
    core/sample/samplereaderwav.cpp \
    core/sample/sampleutils.cpp \
    core/sample/samplewriterwav.cpp \
    core/sample/sound.cpp \
    core/sample/sampleloader.cpp \
    core/duplicator.cpp \
    core/types/serializabletypes.cpp \
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
    editor/modulator/modulatorsplitter.cpp \
    editor/page.cpp \
    editor/pagetable.cpp \
    editor/envelopeditor.cpp \
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
    editor/tree/treesplitter.cpp \
    editor/widgets/equalizer.cpp \
    editor/widgets/tableheaderviewv.cpp \
    editor/widgets/tablewidget.cpp \
    editor/widgets/tabledelegate.cpp \
    editor/widgets/spinboxkey.cpp \
    editor/widgets/spinboxrange.cpp \
    editor/widgets/comboboxloopmode.cpp \
    editor/tools/division_duplication/duplicationtool.cpp \
    clavier/pianoscene.cpp \
    clavier/pianokey.cpp \
    clavier/pianokeybd.cpp \
    clavier/pianokeybdcustom.cpp \
    clavier/keyboardmap.cpp \
    editor/widgets/transparentframe.cpp \
    repository/widgets/customtextedit.cpp \
    repository/soundfont/editor/editordialoginsertlink.cpp \
    repository/soundfont/editor/editordialoginserttable.cpp \
    repository/soundfont/editor/htmleditor.cpp \
    repository/soundfont/editor/soundfonteditorfiles.cpp \
    repository/soundfont/editor/soundfontfilecell.cpp \
    sound_engine/elements/liveeq.cpp \
    sound_engine/modulatedparameter.cpp \
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
    core/input/inputfactory.cpp \
    core/input/sf2/sf2header.cpp \
    core/input/sf2/sf2sdtapart.cpp \
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
    repository/soundfontdetails.cpp \
    repository/detailsmanager.cpp \
    repository/user/userarea.cpp \
    repository/urlreaderimage.cpp \
    repository/widgets/elidedpushbutton.cpp \
    repository/soundfont/soundfontviewer.cpp \
    repository/soundfont/soundfontcommentdata.cpp \
    repository/soundfont/soundfontdescriptiondata.cpp \
    repository/soundfont/soundfontdownloaddata.cpp \
    repository/soundfont/viewer/soundfontdownload.cpp \
    repository/soundfont/viewer/soundfontcomment.cpp \
    repository/soundfont/viewer/soundfontdownloadcell.cpp \
    repository/soundfont/viewer/soundfontcommentcell.cpp \
    repository/soundfont/viewer/soundfontviewerleft.cpp \
    repository/soundfont/viewer/soundfontviewercenter.cpp \
    repository/soundfont/viewer/soundfontviewertop.cpp \
    repository/soundfont/editor/soundfonteditorleft.cpp \
    repository/soundfont/editor/soundfonteditorcenter.cpp \
    repository/soundfont/editor/soundfonteditortop.cpp \
    repository/downloadmanager.cpp \
    repository/widgets/downloadprogressbutton.cpp \
    repository/widgets/downloadprogresscell.cpp \
    editor/overview/sortedtablewidgetitem.cpp \
    repository/widgets/elidedlabel.cpp \
    mainwindow/animatedbackground.cpp \
    editor/modulator/modulatoreditor.cpp \
    editor/modulator/modulatorcell.cpp \
    editor/modulator/modulatorcombocurve.cpp \
    editor/modulator/modulatorcombodest.cpp \
    editor/modulator/modulatorcombosrc.cpp \
    clavier/controllerarea.cpp \
    clavier/combocc.cpp \
    lib/iir/Biquad.cpp \
    lib/iir/Butterworth.cpp \
    lib/iir/Cascade.cpp \
    lib/iir/PoleFilter.cpp \
    lib/iir/State.cpp \
    editor/widgets/spinboxcents.cpp \
    sound_engine/parametermodulator.cpp \
    sound_engine/modulatorgroup.cpp \
    editor/modulator/modulatorlistwidget.cpp \
    clavier/styledslider.cpp \
    editor/tools/default_mod/tooldefaultmod.cpp \
    editor/tools/default_mod/tooldefaultmod_parameters.cpp \
    editor/tools/default_mod/tooldefaultmod_gui.cpp \
    core/types/modulatordata.cpp \
    repository/soundfont/editor/editordialoginsertvideo.cpp \
    repository/soundfont/editor/editordialoginsertimage.cpp \
    dialogs/dialogquestion.cpp \
    editor/graphics/graphicswave.cpp \
    editor/graphics/graphicswavepainter.cpp \
    dialogs/dialognewelement.cpp \
    editor/tools/fast_edit_smpl/toolfasteditsmpl.cpp \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_gui.cpp \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_parameters.cpp \
    repository/soundfont/uploadingdialog.cpp

HEADERS += \
    core/input/grandorgue/grandorguedatathrough.h \
    core/input/grandorgue/grandorgueranklink.h \
    core/solomanager.h \
    core/input/abstractinput.h \
    core/input/abstractinputparser.h \
    core/input/empty/inputparserempty.h \
    core/input/grandorgue/grandorguepipe.h \
    core/input/grandorgue/grandorguerank.h \
    core/input/grandorgue/grandorguestop.h \
    core/input/grandorgue/inputgrandorgue.h \
    core/input/grandorgue/inputparsergrandorgue.h \
    core/input/not_supported/inputparsernotsupported.h \
    core/input/sf2/inputparsersf2.h \
    core/input/sf2/inputsf2.h \
    core/input/sf3/inputparsersf3.h \
    core/input/sf3/inputsf3.h \
    core/input/sfark/inputparsersfark.h \
    core/input/sfark/inputsfark.h \
    core/input/sfz/inputparsersfz.h \
    core/input/sfz/inputsfz.h \
    core/sample/infosound.h \
    core/sample/samplereader.h \
    core/sample/samplereaderfactory.h \
    core/sample/samplereaderflac.h \
    core/sample/samplereadersf2.h \
    core/sample/samplereaderwav.h \
    core/sample/sampleutils.h \
    core/sample/samplewriterwav.h \
    core/sample/sound.h \
    core/sample/sampleloader.h \
    core/duplicator.h \
    core/types/serializabletypes.h \
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
    editor/modulator/modulatorsplitter.h \
    editor/page.h \
    editor/pagetable.h \
    editor/envelopeditor.h \
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
    editor/tree/treesplitter.h \
    editor/widgets/equalizer.h \
    editor/widgets/tableheaderviewv.h \
    editor/widgets/tablewidget.h \
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
    editor/widgets/transparentframe.h \
    repository/widgets/customtextedit.h \
    repository/soundfont/editor/editordialoginsertlink.h \
    repository/soundfont/editor/editordialoginserttable.h \
    repository/soundfont/editor/htmleditor.h \
    repository/soundfont/editor/soundfonteditorfiles.h \
    repository/soundfont/editor/soundfontfilecell.h \
    sound_engine/elements/liveeq.h \
    sound_engine/modulatedparameter.h \
    sound_engine/synth.h \
    sound_engine/voice.h \
    sound_engine/circularbuffer.h \
    sound_engine/voiceparam.h \
    sound_engine/soundengine.h \
    sound_engine/elements/calibrationsinus.h \
    sound_engine/elements/enveloppevol.h \
    sound_engine/elements/oscsinus.h \
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
    core/input/inputfactory.h \
    core/input/sf2/sf2header.h \
    core/input/sf2/sf2sdtapart.h \
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
    repository/widgets/elidedpushbutton.h \
    repository/soundfontdetails.h \
    repository/detailsmanager.h \
    repository/urlreaderimage.h \
    repository/soundfont/soundfontviewer.h \
    repository/soundfont/soundfontcommentdata.h \
    repository/soundfont/soundfontdescriptiondata.h \
    repository/soundfont/soundfontdownloaddata.h \
    repository/soundfont/viewer/soundfontdownload.h \
    repository/soundfont/viewer/soundfontcomment.h \
    repository/soundfont/viewer/soundfontdownloadcell.h \
    repository/soundfont/viewer/soundfontcommentcell.h \
    repository/soundfont/viewer/soundfontviewerleft.h \
    repository/soundfont/viewer/soundfontviewercenter.h \
    repository/soundfont/viewer/soundfontviewertop.h \
    repository/soundfont/editor/smallcombobox.h \
    repository/soundfont/editor/soundfonteditorleft.h \
    repository/soundfont/editor/soundfonteditorcenter.h \
    repository/soundfont/editor/soundfonteditortop.h \
    repository/downloadmanager.h \
    repository/widgets/downloadprogressbutton.h \
    repository/widgets/downloadprogresscell.h \
    core/types/indexedelementlist.h \
    editor/overview/sortedtablewidgetitem.h \
    mainwindow/animatedbackground.h \
    editor/modulator/modulatoreditor.h \
    editor/modulator/modulatorcell.h \
    editor/modulator/modulatorcombocurve.h \
    editor/modulator/modulatorcombodest.h \
    editor/modulator/modulatorcombosrc.h \
    editor/modulator/modulatorlistwidget.h \
    context/polypressureevent.h \
    context/monopressureevent.h \
    context/bendevent.h \
    context/programevent.h \
    clavier/controllerarea.h \
    clavier/combocc.h \
    lib/iir/Iir_2.h \
    lib/iir/Biquad.h \
    lib/iir/Butterworth.h \
    lib/iir/Cascade.h \
    lib/iir/Common.h \
    lib/iir/Layout.h \
    lib/iir/MathSupplement.h \
    lib/iir/PoleFilter.h \
    lib/iir/State.h \
    lib/iir/Types.h \
    editor/widgets/spinboxcents.h \
    sound_engine/parametermodulator.h \
    sound_engine/modulatorgroup.h \
    core/types/modulatordata.h \
    clavier/styledslider.h \
    editor/tools/default_mod/tooldefaultmod.h \
    editor/tools/default_mod/tooldefaultmod_parameters.h \
    editor/tools/default_mod/tooldefaultmod_gui.h \
    repository/soundfont/editor/editordialoginsertvideo.h \
    repository/soundfont/editor/editordialoginsertimage.h \
    dialogs/dialogquestion.h \
    editor/modulator/spinboxmultiply.h \
    editor/graphics/graphicswave.h \
    editor/graphics/graphicswavepainter.h \
    dialogs/dialognewelement.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_gui.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_parameters.h \
    repository/soundfont/uploadingdialog.h

FORMS += \
    dialogs/dialog_list.ui \
    dialogs/dialog_rename.ui \
    dialogs/dialog_about.ui \
    dialogs/dialogselection.ui \
    dialogs/dialogchangelog.ui \
    editor/envelopeditor.ui \
    editor/overview/pageoverview.ui \
    editor/widgets/equalizer.ui \
    mainwindow/widgetshowhistory.ui \
    mainwindow/widgetshowhistorycell.ui \
    mainwindow/mainwindow.ui \
    repository/browser/soundfontbrowser.ui \
    repository/browser/filtertag.ui \
    repository/browser/soundfontcellfull.ui \
    repository/browser/filtertagcell.ui \
    repository/daily/showsoundfonts.ui \
    repository/daily/soundfontcell.ui \
    repository/soundfont/editor/editordialoginsertlink.ui \
    repository/soundfont/editor/editordialoginserttable.ui \
    repository/soundfont/editor/htmleditor.ui \
    repository/soundfont/editor/soundfonteditorfiles.ui \
    repository/soundfont/editor/soundfontfilecell.ui \
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
    repository/soundfont/viewer/soundfontdownload.ui \
    repository/soundfont/viewer/soundfontcomment.ui \
    repository/soundfont/viewer/soundfontdownloadcell.ui \
    repository/soundfont/viewer/soundfontcommentcell.ui \
    repository/soundfont/viewer/soundfontviewerleft.ui \
    repository/soundfont/viewer/soundfontviewercenter.ui \
    repository/soundfont/viewer/soundfontviewertop.ui \
    repository/soundfont/editor/soundfonteditorleft.ui \
    repository/soundfont/editor/soundfonteditorcenter.ui \
    repository/soundfont/editor/soundfonteditortop.ui \
    repository/widgets/downloadprogresscell.ui \
    editor/modulator/modulatoreditor.ui \
    editor/modulator/modulatorcell.ui \
    clavier/controllerarea.ui \
    editor/tools/default_mod/tooldefaultmod_gui.ui \
    repository/soundfont/editor/editordialoginsertvideo.ui \
    repository/soundfont/editor/editordialoginsertimage.ui \
    dialogs/dialogquestion.ui \
    dialogs/dialognewelement.ui \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_gui.ui \
    repository/soundfont/uploadingdialog.ui

DISTFILES += \
    changelog

RESOURCES += resources.qrc
