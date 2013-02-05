#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:06:08
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32{QT += multimedia}
unix {CONFIG += mobility
      MOBILITY += multimedia}

TARGET = polyphone
TEMPLATE = app

release{
    DESTDIR = RELEASE
    OBJECTS_DIR = RELEASE/.obj
    MOC_DIR = RELEASE/.moc
    RCC_DIR = RELEASE/.rcc
    UI_DIR = RELEASE/.ui
} else {
    DESTDIR = DEBUG
    OBJECTS_DIR = DEBUG/.obj
    MOC_DIR = DEBUG/.moc
    RCC_DIR = DEBUG/.rcc
    UI_DIR = DEBUG/.ui}

INCLUDEPATH += gui_divers \
    pages \
    qcustomplot \
    ressources \
    sf2_core \
    tools \
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
    pages/audio.cpp \
    qcustomplot/qcustomplot.cpp \
    tools/dialog_space.cpp \
    tools/dialog_paramglobal.cpp \
    tools/dialog_mixture.cpp \
    tools/dialog_sifflements.cpp

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
    pages/audio.h \
    qcustomplot/qcustomplot.h \
    tools/dialog_space.h \
    tools/dialog_paramglobal.h \
    tools/dialog_mixture.h \
    tools/dialog_sifflements.h

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

OTHER_FILES +=
    
RESOURCES += \
    ressources.qrc

TRANSLATIONS = polyphone_en.ts

RC_FILE = polyphone.rc
