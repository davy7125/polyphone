/********************************************************************************
** Form generated from reading UI file 'dialog_sifflements.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_SIFFLEMENTS_H
#define UI_DIALOG_SIFFLEMENTS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogSifflements
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_3;
    QSpinBox *spinFreq1;
    QSpinBox *spinFreq2;
    QDoubleSpinBox *doubleSpinRaideur;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogSifflements)
    {
        if (DialogSifflements->objectName().isEmpty())
            DialogSifflements->setObjectName(QString::fromUtf8("DialogSifflements"));
        DialogSifflements->setWindowModality(Qt::WindowModal);
        DialogSifflements->resize(283, 142);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogSifflements->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(DialogSifflements);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_2 = new QLabel(DialogSifflements);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_4->addWidget(label_2);

        label = new QLabel(DialogSifflements);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_4->addWidget(label);

        label_3 = new QLabel(DialogSifflements);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);


        horizontalLayout_3->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        spinFreq1 = new QSpinBox(DialogSifflements);
        spinFreq1->setObjectName(QString::fromUtf8("spinFreq1"));
        spinFreq1->setMaximum(20000);
        spinFreq1->setValue(8000);

        verticalLayout_3->addWidget(spinFreq1);

        spinFreq2 = new QSpinBox(DialogSifflements);
        spinFreq2->setObjectName(QString::fromUtf8("spinFreq2"));
        spinFreq2->setMaximum(20000);
        spinFreq2->setValue(20000);

        verticalLayout_3->addWidget(spinFreq2);

        doubleSpinRaideur = new QDoubleSpinBox(DialogSifflements);
        doubleSpinRaideur->setObjectName(QString::fromUtf8("doubleSpinRaideur"));
        doubleSpinRaideur->setMaximum(10);
        doubleSpinRaideur->setValue(5);

        verticalLayout_3->addWidget(doubleSpinRaideur);


        horizontalLayout_3->addLayout(verticalLayout_3);


        verticalLayout->addLayout(horizontalLayout_3);

        buttonBox = new QDialogButtonBox(DialogSifflements);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DialogSifflements);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogSifflements, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogSifflements, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogSifflements);
    } // setupUi

    void retranslateUi(QDialog *DialogSifflements)
    {
        DialogSifflements->setWindowTitle(QApplication::translate("DialogSifflements", "Diminuer sifflements", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DialogSifflements", "D\303\251but de la coupure (Hz)", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogSifflements", "Fin de la coupure (Hz)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DialogSifflements", "Raideur (0-10)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogSifflements: public Ui_DialogSifflements {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_SIFFLEMENTS_H
