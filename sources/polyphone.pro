#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

# Use local copies of RtAudio, RtMidi, and Stk libraries
# (this is forced to true for Windows or Mac OS X)
# Uncomment a line if your distribution doesn't come with some of the following libraries
DEFINES += USE_LOCAL_RTAUDIO
#DEFINES += USE_LOCAL_RTMIDI
#DEFINES += USE_LOCAL_STK

# Uncomment this line to use wolfssl instead of openssl
#DEFINES += USE_WOLFSSL

# Uncomment this line to be compatible with RtAudio version 5.2 (instead of 6)
#DEFINES += RT_AUDIO_5_2

# Uncomment lines for hiding features
#DEFINES += NO_SF2_REPOSITORY
#DEFINES += NO_SF2_CREATION

# Polyphone version
DEFINES += SOFT_VERSION=\\\"2.6.0\\\"
DEFINES += IDENTIFIER=\\\"BETA\\\"
DEFINES += CURRENT_YEAR=\\\"2025\\\"

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
    polyphone_da.ts \
    polyphone_ru.ts \
    polyphone_ko.ts \
    polyphone_ca.ts \
    polyphone_pl.ts

PRECOMPILED_HEADER = precompiled_header.h
CONFIG += lrelease embed_translations precompiled_header
QMAKE_LRELEASE_FLAGS = -nounfinished -removeidentical

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -std=c++17
} else {
    QMAKE_CXXFLAGS += -std=c++17 -O3
    #QMAKE_CXXFLAGS += -std=c++17 -O3 -mcpu=native -march=native
}

QT += core gui printsupport svg network #testlib
TARGET = polyphone
TEMPLATE = app

win32 {
    # Compiler must be MinGW for the option -ffloat-store, required by sfArk
    DEFINES += USE_LOCAL_RTAUDIO USE_LOCAL_RTMIDI USE_LOCAL_STK \
        __WINDOWS_MM__ __WINDOWS_WASAPI__ __WINDOWS_ASIO__ #__WINDOWS_DS__
    INCLUDEPATH += ../windows/include
    RC_FILE = polyphone.rc
    QMAKE_CXXFLAGS += -ffloat-store
    LIBS += -lz -lwinmm -logg -lvorbis -lvorbisfile -lvorbisenc -lcrypto -lFLAC -lsndfile \
        -lole32 -lwinmm -lksuser -lmfplat -lmfuuid -lwmcodecdspuuid # <- for RtAudio
    LIBS += -L$$PWD/../windows/lib
    DESTDIR = $$PWD/../windows/bin

    # Files necessary for ASIO with RtAudio
    HEADERS += lib/_option_rtaudio/rtaudio/include/asio.h \
        lib/_option_rtaudio/rtaudio/include/asiodrivers.h \
        lib/_option_rtaudio/rtaudio/include/asiodrvr.h \
        lib/_option_rtaudio/rtaudio/include/asiolist.h \
        lib/_option_rtaudio/rtaudio/include/asiosys.h \
        lib/_option_rtaudio/rtaudio/include/dsound.h \
        lib/_option_rtaudio/rtaudio/include/functiondiscoverykeys_devpkey.h \
        lib/_option_rtaudio/rtaudio/include/ginclude.h \
        lib/_option_rtaudio/rtaudio/include/iasiodrv.h \
        lib/_option_rtaudio/rtaudio/include/iasiothiscallresolver.h \
        lib/_option_rtaudio/rtaudio/include/soundcard.h
    SOURCES += lib/_option_rtaudio/rtaudio/include/asio.cpp \
        lib/_option_rtaudio/rtaudio/include/asiodrivers.cpp \
        lib/_option_rtaudio/rtaudio/include/asiolist.cpp \
        lib/_option_rtaudio/rtaudio/include/iasiothiscallresolver.cpp
}
unix:!macx {
    DEFINES += __LINUX_ALSASEQ__ __UNIX_JACK__ __LINUX_ALSA__ #__LINUX_PULSE__
    CONFIG += link_pkgconfig
    PKGCONFIG += alsa jack zlib ogg vorbis vorbisfile vorbisenc sndfile
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
    install_image_png.path = $$PREFIX/share/icons/hicolor/512x512/apps
    install_image_png.files = resources/polyphone.png
    install_desktop.path = $$PREFIX/share/applications
    install_desktop.files = contrib/io.polyphone.polyphone.desktop
    install_appdata.path = $$PREFIX/share/metainfo
    install_appdata.files = contrib/io.polyphone.polyphone.metainfo.xml
    install_mime.path = $$PREFIX/share/mime/packages
    install_mime.files = contrib/polyphone.xml
    install_man.path = $$PREFIX/share
    install_man.files = contrib/man
    install_doc.path = $$PREFIX/share/doc/polyphone
    install_doc.files = ../README.md changelog
    
    INSTALLS += target install_image_png \
                install_desktop install_appdata install_mime install_man install_doc

    # Remove the default RPATH
    QMAKE_LFLAGS_RPATH=
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    QMAKE_MAC_SDK = macosx14.4
    DEFINES += USE_LOCAL_RTAUDIO USE_LOCAL_RTMIDI USE_LOCAL_STK \
        __MACOSX_CORE__ TARGET_OS_IPHONE=0
    INCLUDEPATH += ../macos/include
    LIBS += -L$$PWD/../macos/lib -lssl -lcrypto -lz \
        -lsndfile -lopus -lmpg123 -logg -lFLAC -lvorbis -lvorbisfile -lvorbisenc -lmp3lame \
        -framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
        -framework AudioUnit -framework AudioToolbox -framework Cocoa
    ICON = contrib/mac/polyphone.icns
    QMAKE_INFO_PLIST = contrib/mac/polyphone.plist
    DESTDIR = $$PWD/../macos
}
DEFINES += SFTOOLS_NOXML

# Location of RtAudio
contains(DEFINES, USE_LOCAL_RTAUDIO) {
    DEFINES -= RT_AUDIO_5_2
    INCLUDEPATH += lib/_option_rtaudio \
        lib/_option_rtaudio/rtaudio/include
    HEADERS += lib/_option_rtaudio/rtaudio/RtAudio.h
    SOURCES += lib/_option_rtaudio/rtaudio/RtAudio.cpp
} else {
    PKGCONFIG += rtaudio
}

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

INCLUDEPATH += lib \
    mainwindow \
    dialogs \
    context \
    context/interface \
    editor \
    editor/footers \
    editor/graphics \
    editor/modulator \
    editor/overview \
    editor/tools \
    editor/widgets \
    editor/tree \
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
    lib/qtsingleapplication \
    extensions \
    player \
    .

SOURCES	+= main.cpp \
    core/input/sf/inputparsersf.cpp \
    core/sample/samplereadersf.cpp \
    context/tuningprogrammanager.cpp \
    core/fastmaths.cpp \
    core/input/grandorgue/grandorguedatathrough.cpp \
    core/input/grandorgue/grandorgueranklink.cpp \
    core/input/grandorgue/grandorgueswitch.cpp \
    core/input/sf/inputsf.cpp \
    core/input/sfz/sfzparametergroup.cpp \
    core/input/sfz/sfzparameterregion.cpp \
    core/output/sfz/balanceparameters.cpp \
    core/output/sfz/sfzwriter.cpp \
    core/sample/samplereaderfile.cpp \
    core/solomanager.cpp \
    core/input/abstractinputparser.cpp \
    core/input/empty/inputparserempty.cpp \
    core/input/grandorgue/grandorguepipe.cpp \
    core/input/grandorgue/grandorguerank.cpp \
    core/input/grandorgue/grandorguestop.cpp \
    core/input/grandorgue/inputgrandorgue.cpp \
    core/input/grandorgue/inputparsergrandorgue.cpp \
    core/input/not_supported/inputparsernotsupported.cpp \
    core/input/sfark/inputparsersfark.cpp \
    core/input/sfark/inputsfark.cpp \
    core/input/sfz/inputparsersfz.cpp \
    core/input/sfz/inputsfz.cpp \
    core/sample/samplereaderfactory.cpp \
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
    editor/footers/footerlinkedto.cpp \
    editor/footers/footeroverview.cpp \
    editor/footers/footerprst.cpp \
    editor/footers/footersf2.cpp \
    editor/graphics/graphicsfourier.cpp \
    editor/modulator/modulatorsplitter.cpp \
    editor/overview/pageoverviewsampledelegate.cpp \
    editor/page.cpp \
    editor/pageenvelope.cpp \
    editor/pagerange.cpp \
    editor/pageselector.cpp \
    editor/pagetable.cpp \
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
    editor/tools/loop_crossfade/toolloopcrossfade.cpp \
    editor/tools/loop_crossfade/toolloopcrossfade_gui.cpp \
    editor/tools/loop_crossfade/toolloopcrossfade_parameters.cpp \
    editor/tools/merge_samples/runnablemerger.cpp \
    editor/tools/merge_samples/toolmergesamples.cpp \
    editor/tools/merge_samples/toolmergesamples_gui.cpp \
    editor/tools/merge_samples/toolmergesamples_parameters.cpp \
    editor/tools/monitor/segmentpainter.cpp \
    editor/tree/treesplitter.cpp \
    editor/widgets/equalizer.cpp \
    editor/widgets/nullablespinbox.cpp \
    editor/widgets/pushstereoediting.cpp \
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
    extensions/extension_midi_dialog.cpp \
    extensions/extensionmanager.cpp \
    extensions/extensionmanager_midi.cpp \
    extensions/extensionmanager_view.cpp \
    mainwindow/mainstackedwidget.cpp \
    mainwindow/maintabbar.cpp \
    mainwindow/maintabbarelement.cpp \
    mainwindow/tab.cpp \
    mainwindow/tabmanager.cpp \
    mainwindow/topbackground.cpp \
    player/player.cpp \
    player/playeroptions.cpp \
    player/playerpresetlist.cpp \
    player/playerpresetlistdelegate.cpp \
    player/playertreeproxymodel.cpp \
    repository/browser/browsersortmenu.cpp \
    repository/widgets/customtextedit.cpp \
    repository/soundfont/editor/editordialoginsertlink.cpp \
    repository/soundfont/editor/editordialoginserttable.cpp \
    repository/soundfont/editor/htmleditor.cpp \
    repository/soundfont/editor/soundfonteditorfiles.cpp \
    repository/soundfont/editor/soundfontfilecell.cpp \
    sound_engine/elements/liveeq.cpp \
    sound_engine/elements/osctriangle.cpp \
    sound_engine/elements/recorder.cpp \
    sound_engine/modulatedparameter.cpp \
    sound_engine/synth.cpp \
    sound_engine/voice.cpp \
    sound_engine/voicelist.cpp \
    sound_engine/voiceparam.cpp \
    sound_engine/soundengine.cpp \
    sound_engine/elements/calibrationsinus.cpp \
    sound_engine/elements/enveloppevol.cpp \
    sound_engine/elements/oscsinus.cpp \
    options.cpp \
    mainwindow/widgetshowhistory.cpp \
    mainwindow/widgetshowhistorycell.cpp \
    mainwindow/mainwindow.cpp \
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
    editor/widgets/editortoolbar.cpp \
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
    core/actionset.cpp \
    core/action.cpp \
    editor/widgets/styledlineedit.cpp \
    core/input/inputfactory.cpp \
    core/input/sf/sf2header.cpp \
    core/input/sf/sf2sdtapart.cpp \
    core/input/sf/sf2pdtapart.cpp \
    core/input/sf/sf2pdtapart_phdr.cpp \
    core/input/sf/sf2pdtapart_inst.cpp \
    core/input/sf/sf2pdtapart_shdr.cpp \
    core/input/sf/sf2pdtapart_mod.cpp \
    core/input/sf/sf2pdtapart_gen.cpp \
    core/input/sf/sf2pdtapart_bag.cpp \
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
    core/output/sfz/outputsfz.cpp \
    core/output/not_supported/outputnotsupported.cpp \
    core/output/sfz/sfzparamlist.cpp \
    core/input/sfz/sfzparameter.cpp \
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
    repository/soundfont/uploadingdialog.cpp \
    lib/qtsingleapplication/qtlocalpeer.cpp \
    lib/qtsingleapplication/qtsingleapplication.cpp \
    editor/tools/load_from_inst/toolloadfrominst.cpp \
    editor/tools/load_from_inst/toolloadfrominst_gui.cpp \
    editor/tools/load_from_inst/toolloadfrominst_parameters.cpp \
    core/input/sfark/sfarkextractor1.cpp \
    core/input/sfark/sfarkextractor2.cpp \
    core/output/sf/outputsf.cpp \
    core/output/sf/sfindexconverter.cpp

HEADERS += \
    context/imidilistener.h \
    core/input/sf/inputparsersf.h \
    core/sample/samplereadersf.h \
    context/tuningprogrammanager.h \
    core/fastmaths.h \
    core/input/grandorgue/grandorguedatathrough.h \
    core/input/grandorgue/grandorgueranklink.h \
    core/input/grandorgue/grandorgueswitch.h \
    core/input/sf/inputsf.h \
    core/input/sfz/sfzparametergroup.h \
    core/input/sfz/sfzparameterregion.h \
    core/output/sfz/balanceparameters.h \
    core/output/sfz/sfzwriter.h \
    core/sample/samplereaderfile.h \
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
    core/input/sfark/inputparsersfark.h \
    core/input/sfark/inputsfark.h \
    core/input/sfz/inputparsersfz.h \
    core/input/sfz/inputsfz.h \
    core/sample/infosound.h \
    core/sample/samplereader.h \
    core/sample/samplereaderfactory.h \
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
    editor/footers/abstractfooter.h \
    editor/footers/footerlinkedto.h \
    editor/footers/footeroverview.h \
    editor/footers/footerprst.h \
    editor/footers/footersf2.h \
    editor/graphics/graphicsfourier.h \
    editor/modulator/modulatorsplitter.h \
    editor/overview/pageoverviewsampledelegate.h \
    editor/page.h \
    editor/pageenvelope.h \
    editor/pagerange.h \
    editor/pageselector.h \
    editor/pagetable.h \
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
    editor/tools/loop_crossfade/toolloopcrossfade.h \
    editor/tools/loop_crossfade/toolloopcrossfade_gui.h \
    editor/tools/loop_crossfade/toolloopcrossfade_parameters.h \
    editor/tools/merge_samples/runnablemerger.h \
    editor/tools/merge_samples/toolmergesamples.h \
    editor/tools/merge_samples/toolmergesamples_gui.h \
    editor/tools/merge_samples/toolmergesamples_parameters.h \
    editor/tools/monitor/segment.h \
    editor/tools/monitor/segmentpainter.h \
    editor/tree/treesplitter.h \
    editor/widgets/equalizer.h \
    editor/widgets/pushstereoediting.h \
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
    extensions/extension_midi.h \
    extensions/extension_midi_dialog.h \
    extensions/extension_view.h \
    extensions/extensionmanager.h \
    extensions/extensionmanager_midi.h \
    extensions/extensionmanager_view.h \
    mainwindow/mainstackedwidget.h \
    mainwindow/maintabbar.h \
    mainwindow/maintabbarelement.h \
    mainwindow/tab.h \
    mainwindow/tabmanager.h \
    mainwindow/topbackground.h \
    player/player.h \
    player/playeroptions.h \
    player/playerpresetlist.h \
    player/playerpresetlistdelegate.h \
    player/playertreeproxymodel.h \
    repository/browser/browsersortmenu.h \
    repository/widgets/customtextedit.h \
    repository/soundfont/editor/editordialoginsertlink.h \
    repository/soundfont/editor/editordialoginserttable.h \
    repository/soundfont/editor/htmleditor.h \
    repository/soundfont/editor/soundfonteditorfiles.h \
    repository/soundfont/editor/soundfontfilecell.h \
    sound_engine/elements/liveeq.h \
    sound_engine/elements/osctriangle.h \
    sound_engine/elements/recorder.h \
    sound_engine/imidivalues.h \
    sound_engine/modulatedparameter.h \
    sound_engine/synth.h \
    sound_engine/voice.h \
    sound_engine/voicelist.h \
    sound_engine/voiceparam.h \
    sound_engine/soundengine.h \
    sound_engine/elements/calibrationsinus.h \
    sound_engine/elements/enveloppevol.h \
    sound_engine/elements/oscsinus.h \
    options.h \
    mainwindow/widgetshowhistory.h \
    mainwindow/widgetshowhistorycell.h \
    mainwindow/mainwindow.h \
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
    editor/widgets/editortoolbar.h\
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
    core/actionset.h \
    core/action.h \
    editor/widgets/styledlineedit.h \
    core/input/inputfactory.h \
    core/input/sf/sf2header.h \
    core/input/sf/sf2sdtapart.h \
    core/input/sf/sf2pdtapart.h \
    core/input/sf/sf2pdtapart_phdr.h \
    core/input/sf/sf2pdtapart_inst.h \
    core/input/sf/sf2pdtapart_shdr.h \
    core/input/sf/sf2pdtapart_mod.h \
    core/input/sf/sf2pdtapart_gen.h \
    core/input/sf/sf2pdtapart_bag.h \
    repository/widgets/elidedlabel.h \
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
    core/output/sfz/outputsfz.h \
    core/output/not_supported/outputnotsupported.h \
    core/output/sfz/sfzparamlist.h \
    core/input/sfz/sfzparameter.h \
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
    editor/modulator/modulatoreditor.h \
    editor/modulator/modulatorcell.h \
    editor/modulator/modulatorcombocurve.h \
    editor/modulator/modulatorcombodest.h \
    editor/modulator/modulatorcombosrc.h \
    editor/modulator/modulatorlistwidget.h \
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
    editor/graphics/graphicswave.h \
    editor/graphics/graphicswavepainter.h \
    dialogs/dialognewelement.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_gui.h \
    editor/tools/fast_edit_smpl/toolfasteditsmpl_parameters.h \
    repository/soundfont/uploadingdialog.h \
    dialogs/latinvalidator.h \
    editor/widgets/nullablespinbox.h \
    lib/qtsingleapplication/qtlocalpeer.h \
    lib/qtsingleapplication/qtsingleapplication.h \
    editor/tools/load_from_inst/toolloadfrominst.h \
    editor/tools/load_from_inst/toolloadfrominst_gui.h \
    editor/tools/load_from_inst/toolloadfrominst_parameters.h \
    core/input/sfark/sfarkextractor2.h \
    core/input/sfark/abstractextractor.h \
    core/output/sf/outputsf.h \
    core/output/sf/sfindexconverter.h

FORMS += \
    dialogs/dialog_list.ui \
    dialogs/dialog_rename.ui \
    dialogs/dialog_about.ui \
    dialogs/dialogselection.ui \
    dialogs/dialogchangelog.ui \
    editor/footers/footerlinkedto.ui \
    editor/footers/footeroverview.ui \
    editor/footers/footerprst.ui \
    editor/footers/footersf2.ui \
    editor/overview/pageoverview.ui \
    editor/pageenvelope.ui \
    editor/pagerange.ui \
    editor/tools/loop_crossfade/toolloopcrossfade_gui.ui \
    editor/tools/merge_samples/toolmergesamples_gui.ui \
    editor/widgets/editortoolbar.ui \
    editor/widgets/equalizer.ui \
    mainwindow/widgetshowhistory.ui \
    mainwindow/widgetshowhistorycell.ui \
    mainwindow/mainwindow.ui \
    player/player.ui \
    repository/browser/browsersortmenu.ui \
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
    editor/tools/load_from_inst/toolloadfrominst_gui.ui \
    repository/soundfont/uploadingdialog.ui

DISTFILES += \
    changelog

RESOURCES += resources.qrc
