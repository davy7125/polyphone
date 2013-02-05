/********************************************************************************
** Form generated from reading UI file 'dialog_space.ui'
**
** Created: Tue Feb 5 22:37:23 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_SPACE_H
#define UI_DIALOG_SPACE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include "dialog_space.h"

QT_BEGIN_NAMESPACE

class Ui_DialogSpace
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_4;
    GraphSpace *grapheSpace;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QComboBox *comboMotif;
    QSpinBox *spinNbDivision;
    QSpinBox *spinEtalement;
    QSpinBox *spinOccupation;
    QSpinBox *spinOffset;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkSens;
    QCheckBox *checkSens2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogSpace)
    {
        if (DialogSpace->objectName().isEmpty())
            DialogSpace->setObjectName(QString::fromUtf8("DialogSpace"));
        DialogSpace->resize(560, 233);
        DialogSpace->setMinimumSize(QSize(560, 233));
        DialogSpace->setMaximumSize(QSize(560, 233));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogSpace->setWindowIcon(icon);
        DialogSpace->setModal(true);
        verticalLayout = new QVBoxLayout(DialogSpace);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, 0, -1);
        frame_3 = new QFrame(DialogSpace);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::Panel);
        frame_3->setFrameShadow(QFrame::Sunken);
        verticalLayout_4 = new QVBoxLayout(frame_3);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        grapheSpace = new GraphSpace(frame_3);
        grapheSpace->setObjectName(QString::fromUtf8("grapheSpace"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(grapheSpace->sizePolicy().hasHeightForWidth());
        grapheSpace->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(grapheSpace);


        horizontalLayout->addWidget(frame_3);

        frame_2 = new QFrame(DialogSpace);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_2);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_2->addWidget(label_3);

        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_2->addWidget(label_4);

        label_5 = new QLabel(frame_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_2->addWidget(label_5);

        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_2->addWidget(label_6);


        horizontalLayout->addWidget(frame_2);

        frame = new QFrame(DialogSpace);
        frame->setObjectName(QString::fromUtf8("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        comboMotif = new QComboBox(frame);
        comboMotif->setObjectName(QString::fromUtf8("comboMotif"));

        verticalLayout_3->addWidget(comboMotif);

        spinNbDivision = new QSpinBox(frame);
        spinNbDivision->setObjectName(QString::fromUtf8("spinNbDivision"));
        spinNbDivision->setMinimum(1);
        spinNbDivision->setMaximum(10);

        verticalLayout_3->addWidget(spinNbDivision);

        spinEtalement = new QSpinBox(frame);
        spinEtalement->setObjectName(QString::fromUtf8("spinEtalement"));
        spinEtalement->setMaximum(100);
        spinEtalement->setValue(100);

        verticalLayout_3->addWidget(spinEtalement);

        spinOccupation = new QSpinBox(frame);
        spinOccupation->setObjectName(QString::fromUtf8("spinOccupation"));
        spinOccupation->setMaximum(100);
        spinOccupation->setValue(100);

        verticalLayout_3->addWidget(spinOccupation);

        spinOffset = new QSpinBox(frame);
        spinOffset->setObjectName(QString::fromUtf8("spinOffset"));
        spinOffset->setMaximum(100);
        spinOffset->setValue(50);

        verticalLayout_3->addWidget(spinOffset);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, -1, -1, 0);
        checkSens = new QCheckBox(frame);
        checkSens->setObjectName(QString::fromUtf8("checkSens"));

        horizontalLayout_2->addWidget(checkSens);

        checkSens2 = new QCheckBox(frame);
        checkSens2->setObjectName(QString::fromUtf8("checkSens2"));

        horizontalLayout_2->addWidget(checkSens2);


        verticalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(frame);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(DialogSpace);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DialogSpace);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogSpace, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogSpace, SLOT(reject()));
        QObject::connect(comboMotif, SIGNAL(currentIndexChanged(int)), DialogSpace, SLOT(motifChanged(int)));
        QObject::connect(spinNbDivision, SIGNAL(valueChanged(int)), DialogSpace, SLOT(nbDivChanged(int)));
        QObject::connect(spinEtalement, SIGNAL(valueChanged(int)), DialogSpace, SLOT(etalementChanged(int)));
        QObject::connect(spinOccupation, SIGNAL(valueChanged(int)), DialogSpace, SLOT(occupationChanged(int)));
        QObject::connect(spinOffset, SIGNAL(valueChanged(int)), DialogSpace, SLOT(offsetChanged(int)));
        QObject::connect(checkSens, SIGNAL(stateChanged(int)), DialogSpace, SLOT(sensChanged(int)));
        QObject::connect(checkSens2, SIGNAL(stateChanged(int)), DialogSpace, SLOT(sens2Changed(int)));

        QMetaObject::connectSlotsByName(DialogSpace);
    } // setupUi

    void retranslateUi(QDialog *DialogSpace)
    {
        DialogSpace->setWindowTitle(QApplication::translate("DialogSpace", "Spatialisation", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogSpace", "<html><head/><body><p>Motif</p></body></html>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DialogSpace", "Nombre divisions", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DialogSpace", "Etalement (%)", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DialogSpace", "Occupation (%)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DialogSpace", "Offset (0-100)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DialogSpace", "Renversements", 0, QApplication::UnicodeUTF8));
        comboMotif->clear();
        comboMotif->insertItems(0, QStringList()
         << QApplication::translate("DialogSpace", "Ascendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogSpace", "Creux", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogSpace", "Descendant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogSpace", "Pointe", 0, QApplication::UnicodeUTF8)
        );
        checkSens->setText(QString());
        checkSens2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DialogSpace: public Ui_DialogSpace {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_SPACE_H
