/********************************************************************************
** Form generated from reading UI file 'config.ui'
**
** Created: Tue Feb 5 22:37:23 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Config
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tabGeneral;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QComboBox *comboRam;
    QLabel *label;
    QComboBox *comboAudioOuput;
    QLabel *label_4;
    QComboBox *comboMidiInput;
    QLabel *label_3;
    QCheckBox *checkBoucle;
    QCheckBox *checkBlanc;
    QWidget *tabSynth;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QSlider *sliderGain;
    QLabel *labelGain;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushFermer;

    void setupUi(QDialog *Config)
    {
        if (Config->objectName().isEmpty())
            Config->setObjectName(QString::fromUtf8("Config"));
        Config->setWindowModality(Qt::WindowModal);
        Config->resize(400, 229);
        Config->setMinimumSize(QSize(400, 229));
        Config->setMaximumSize(QSize(400, 229));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        Config->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(Config);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(Config);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabGeneral = new QWidget();
        tabGeneral->setObjectName(QString::fromUtf8("tabGeneral"));
        gridLayout_2 = new QGridLayout(tabGeneral);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_2 = new QLabel(tabGeneral);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        comboRam = new QComboBox(tabGeneral);
        comboRam->setObjectName(QString::fromUtf8("comboRam"));

        gridLayout_2->addWidget(comboRam, 0, 1, 1, 1);

        label = new QLabel(tabGeneral);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        comboAudioOuput = new QComboBox(tabGeneral);
        comboAudioOuput->setObjectName(QString::fromUtf8("comboAudioOuput"));

        gridLayout_2->addWidget(comboAudioOuput, 1, 1, 1, 1);

        label_4 = new QLabel(tabGeneral);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 2, 0, 1, 1);

        comboMidiInput = new QComboBox(tabGeneral);
        comboMidiInput->setObjectName(QString::fromUtf8("comboMidiInput"));

        gridLayout_2->addWidget(comboMidiInput, 2, 1, 1, 1);

        label_3 = new QLabel(tabGeneral);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(130, 0));

        gridLayout_2->addWidget(label_3, 3, 0, 1, 1);

        checkBoucle = new QCheckBox(tabGeneral);
        checkBoucle->setObjectName(QString::fromUtf8("checkBoucle"));

        gridLayout_2->addWidget(checkBoucle, 3, 1, 1, 1);

        checkBlanc = new QCheckBox(tabGeneral);
        checkBlanc->setObjectName(QString::fromUtf8("checkBlanc"));

        gridLayout_2->addWidget(checkBlanc, 4, 1, 1, 1);

        tabWidget->addTab(tabGeneral, QString());
        tabSynth = new QWidget();
        tabSynth->setObjectName(QString::fromUtf8("tabSynth"));
        gridLayout = new QGridLayout(tabSynth);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        sliderGain = new QSlider(tabSynth);
        sliderGain->setObjectName(QString::fromUtf8("sliderGain"));
        sliderGain->setMinimum(-50);
        sliderGain->setMaximum(50);
        sliderGain->setOrientation(Qt::Horizontal);
        sliderGain->setTickPosition(QSlider::TicksBelow);
        sliderGain->setTickInterval(25);

        horizontalLayout_2->addWidget(sliderGain);

        labelGain = new QLabel(tabSynth);
        labelGain->setObjectName(QString::fromUtf8("labelGain"));
        labelGain->setMinimumSize(QSize(40, 0));

        horizontalLayout_2->addWidget(labelGain);


        gridLayout->addLayout(horizontalLayout_2, 1, 1, 1, 1);

        label_5 = new QLabel(tabSynth);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(130, 0));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        tabWidget->addTab(tabSynth, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushFermer = new QPushButton(Config);
        pushFermer->setObjectName(QString::fromUtf8("pushFermer"));

        horizontalLayout->addWidget(pushFermer);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Config);
        QObject::connect(pushFermer, SIGNAL(clicked()), Config, SLOT(close()));
        QObject::connect(comboAudioOuput, SIGNAL(activated(int)), Config, SLOT(setAudioOutput(int)));
        QObject::connect(comboRam, SIGNAL(currentIndexChanged(int)), Config, SLOT(setRam(int)));
        QObject::connect(checkBoucle, SIGNAL(toggled(bool)), Config, SLOT(setWavAutoLoop(bool)));
        QObject::connect(checkBlanc, SIGNAL(toggled(bool)), Config, SLOT(setWavRemoveBlank(bool)));
        QObject::connect(comboMidiInput, SIGNAL(activated(int)), Config, SLOT(setNumPortMidi(int)));
        QObject::connect(sliderGain, SIGNAL(sliderMoved(int)), labelGain, SLOT(setNum(int)));
        QObject::connect(sliderGain, SIGNAL(sliderMoved(int)), Config, SLOT(setSynthGain(int)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Config);
    } // setupUi

    void retranslateUi(QDialog *Config)
    {
        Config->setWindowTitle(QApplication::translate("Config", "Pr\303\251f\303\251rences", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Config", "Gestion RAM", 0, QApplication::UnicodeUTF8));
        comboRam->clear();
        comboRam->insertItems(0, QStringList()
         << QApplication::translate("Config", "Charger en m\303\251moire lorsque n\303\251cessaire", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Config", "Tout charger en m\303\251moire", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("Config", "Sortie audio", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Config", "Entr\303\251e midi", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Config", "Import fichiers wav", 0, QApplication::UnicodeUTF8));
        checkBoucle->setText(QApplication::translate("Config", "ajuster \303\240 la boucle", 0, QApplication::UnicodeUTF8));
        checkBlanc->setText(QApplication::translate("Config", "enlever le blanc au d\303\251part", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabGeneral), QApplication::translate("Config", "G\303\251n\303\251ral", 0, QApplication::UnicodeUTF8));
        labelGain->setText(QApplication::translate("Config", "0", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Config", "Gain (dB)", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabSynth), QApplication::translate("Config", "Synth\303\251tiseur", 0, QApplication::UnicodeUTF8));
        pushFermer->setText(QApplication::translate("Config", "Fermer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Config: public Ui_Config {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIG_H
