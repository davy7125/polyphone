/********************************************************************************
** Form generated from reading UI file 'dialog_paramglobal.ui'
**
** Created: Tue Feb 5 22:37:23 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_PARAMGLOBAL_H
#define UI_DIALOG_PARAMGLOBAL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include "dialog_paramglobal.h"

QT_BEGIN_NAMESPACE

class Ui_DialogParamGlobal
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QFrame *frame_1;
    QVBoxLayout *verticalLayout_2;
    GraphParamGlobal *graphParamGlobal;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLabel *label_6;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_5;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_4;
    QComboBox *comboMotif;
    QDoubleSpinBox *doubleSpinRaideur;
    QDoubleSpinBox *doubleSpinMin;
    QDoubleSpinBox *doubleSpinMax;
    QComboBox *comboModif;
    QComboBox *comboValeur;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogParamGlobal)
    {
        if (DialogParamGlobal->objectName().isEmpty())
            DialogParamGlobal->setObjectName(QString::fromUtf8("DialogParamGlobal"));
        DialogParamGlobal->resize(560, 233);
        DialogParamGlobal->setMinimumSize(QSize(560, 233));
        DialogParamGlobal->setMaximumSize(QSize(560, 233));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogParamGlobal->setWindowIcon(icon);
        DialogParamGlobal->setModal(true);
        verticalLayout = new QVBoxLayout(DialogParamGlobal);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        frame_1 = new QFrame(DialogParamGlobal);
        frame_1->setObjectName(QString::fromUtf8("frame_1"));
        frame_1->setFrameShape(QFrame::StyledPanel);
        frame_1->setFrameShadow(QFrame::Sunken);
        verticalLayout_2 = new QVBoxLayout(frame_1);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        graphParamGlobal = new GraphParamGlobal(frame_1);
        graphParamGlobal->setObjectName(QString::fromUtf8("graphParamGlobal"));

        verticalLayout_2->addWidget(graphParamGlobal);


        horizontalLayout->addWidget(frame_1);

        frame_2 = new QFrame(DialogParamGlobal);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_2);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_3->addWidget(label_6);

        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_3->addWidget(label_3);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_3->addWidget(label_2);

        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_3->addWidget(label_4);

        label_5 = new QLabel(frame_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_3->addWidget(label_5);


        horizontalLayout->addWidget(frame_2);

        frame_3 = new QFrame(DialogParamGlobal);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        sizePolicy.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy);
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frame_3);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        comboMotif = new QComboBox(frame_3);
        comboMotif->setObjectName(QString::fromUtf8("comboMotif"));

        verticalLayout_4->addWidget(comboMotif);

        doubleSpinRaideur = new QDoubleSpinBox(frame_3);
        doubleSpinRaideur->setObjectName(QString::fromUtf8("doubleSpinRaideur"));
        doubleSpinRaideur->setEnabled(false);
        doubleSpinRaideur->setMinimum(0);
        doubleSpinRaideur->setMaximum(100);
        doubleSpinRaideur->setValue(50);

        verticalLayout_4->addWidget(doubleSpinRaideur);

        doubleSpinMin = new QDoubleSpinBox(frame_3);
        doubleSpinMin->setObjectName(QString::fromUtf8("doubleSpinMin"));
        doubleSpinMin->setMinimum(-20000);
        doubleSpinMin->setMaximum(20000);
        doubleSpinMin->setValue(0);

        verticalLayout_4->addWidget(doubleSpinMin);

        doubleSpinMax = new QDoubleSpinBox(frame_3);
        doubleSpinMax->setObjectName(QString::fromUtf8("doubleSpinMax"));
        doubleSpinMax->setMinimum(-20000);
        doubleSpinMax->setMaximum(20000);
        doubleSpinMax->setValue(1);

        verticalLayout_4->addWidget(doubleSpinMax);

        comboModif = new QComboBox(frame_3);
        comboModif->setObjectName(QString::fromUtf8("comboModif"));

        verticalLayout_4->addWidget(comboModif);

        comboValeur = new QComboBox(frame_3);
        comboValeur->setObjectName(QString::fromUtf8("comboValeur"));

        verticalLayout_4->addWidget(comboValeur);


        horizontalLayout->addWidget(frame_3);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(DialogParamGlobal);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DialogParamGlobal);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogParamGlobal, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogParamGlobal, SLOT(reject()));
        QObject::connect(comboMotif, SIGNAL(currentIndexChanged(int)), DialogParamGlobal, SLOT(indexMotifChanged(int)));
        QObject::connect(doubleSpinRaideur, SIGNAL(valueChanged(double)), DialogParamGlobal, SLOT(raideurChanged(double)));

        comboMotif->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DialogParamGlobal);
    } // setupUi

    void retranslateUi(QDialog *DialogParamGlobal)
    {
        DialogParamGlobal->setWindowTitle(QApplication::translate("DialogParamGlobal", "Param\303\251trage global", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogParamGlobal", "Motif", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DialogParamGlobal", "Raideur", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DialogParamGlobal", "Minimum", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DialogParamGlobal", "Maximum", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DialogParamGlobal", "Modification", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DialogParamGlobal", "Param\303\250tre", 0, QApplication::UnicodeUTF8));
        comboMotif->clear();
        comboMotif->insertItems(0, QStringList()
         << QApplication::translate("DialogParamGlobal", "Manuel", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Lin\303\251aire ascendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Lin\303\251aire descendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Exponentiel ascendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Exponentiel descendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Al\303\251atoire", 0, QApplication::UnicodeUTF8)
        );
        comboModif->clear();
        comboModif->insertItems(0, QStringList()
         << QApplication::translate("DialogParamGlobal", "Ajout", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Multiplication", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Remplacement", 0, QApplication::UnicodeUTF8)
        );
        comboValeur->clear();
        comboValeur->insertItems(0, QStringList()
         << QApplication::translate("DialogParamGlobal", "Att\303\251nuation (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Accordage (demi-tons)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Accordage (centi\303\250mes)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Accordage (scale)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Filtre, fr\303\251quence (Hz)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Filtre, r\303\251sonance (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env delay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env attack (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env hold (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env decay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env sustain (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env release (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env note \342\206\222 hold (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vol env note \342\206\222 decay (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env delay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env attack (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env hold (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env decay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env sustain (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env release (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env note \342\206\222 hold (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod env note \342\206\222 decay (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod LFO delay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod LFO freq (Hz)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod LFO \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Mod LFO \342\206\222 volume (dB)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vib LFO delay (s)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vib LFO freq (Hz)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Vib LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "Chorus (%)", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogParamGlobal", "R\303\251verb\303\251raton (%)", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class DialogParamGlobal: public Ui_DialogParamGlobal {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_PARAMGLOBAL_H
