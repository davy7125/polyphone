/********************************************************************************
** Form generated from reading UI file 'page_sf2.ui'
**
** Created: Tue Feb 5 22:37:23 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_SF2_H
#define UI_PAGE_SF2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "page_sf2.h"

QT_BEGIN_NAMESPACE

class Ui_Page_Sf2
{
public:
    QVBoxLayout *verticalLayout_4;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QVBoxLayout *verticalLayout_11;
    QLineEdit *lineEdit_name;
    QLineEdit *lineEdit_author;
    QLineEdit *lineEdit_copyright;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineEdit_date;
    QPushButton *pushButton_setNow;
    QLabel *label_25;
    QLineEdit *lineEdit_product;
    QCheckBox *check_24bits;
    QLabel *label_26;
    TextEdit *textEdit_Com;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_filename;
    QLabel *label_sfVersion;
    QLabel *label_soundEngine;
    QLabel *label_romVersion;
    QLabel *label_software;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_8;
    QLabel *label_7;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_13;
    QLabel *label_12;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_nbSmpl;
    QLabel *label_unusedSmpl;
    QLabel *label_nbInst;
    QLabel *label_unusedInst;
    QLabel *label_nbPrst;
    QLabel *label_nbInstGen;
    QLabel *label_nbPrstGen;

    void setupUi(QWidget *Page_Sf2)
    {
        if (Page_Sf2->objectName().isEmpty())
            Page_Sf2->setObjectName(QString::fromUtf8("Page_Sf2"));
        Page_Sf2->resize(679, 627);
        verticalLayout_4 = new QVBoxLayout(Page_Sf2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        frame_3 = new QFrame(Page_Sf2);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::Panel);
        frame_3->setFrameShadow(QFrame::Sunken);
        verticalLayout_9 = new QVBoxLayout(frame_3);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, -1, 0, -1);
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, -1, -1, -1);
        label_21 = new QLabel(frame_3);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        verticalLayout_10->addWidget(label_21);

        label_22 = new QLabel(frame_3);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        verticalLayout_10->addWidget(label_22);

        label_23 = new QLabel(frame_3);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        verticalLayout_10->addWidget(label_23);

        label_24 = new QLabel(frame_3);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        verticalLayout_10->addWidget(label_24);


        horizontalLayout_3->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(0, -1, -1, -1);
        lineEdit_name = new QLineEdit(frame_3);
        lineEdit_name->setObjectName(QString::fromUtf8("lineEdit_name"));

        verticalLayout_11->addWidget(lineEdit_name);

        lineEdit_author = new QLineEdit(frame_3);
        lineEdit_author->setObjectName(QString::fromUtf8("lineEdit_author"));

        verticalLayout_11->addWidget(lineEdit_author);

        lineEdit_copyright = new QLineEdit(frame_3);
        lineEdit_copyright->setObjectName(QString::fromUtf8("lineEdit_copyright"));

        verticalLayout_11->addWidget(lineEdit_copyright);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(-1, 0, -1, -1);
        lineEdit_date = new QLineEdit(frame_3);
        lineEdit_date->setObjectName(QString::fromUtf8("lineEdit_date"));

        horizontalLayout_7->addWidget(lineEdit_date);

        pushButton_setNow = new QPushButton(frame_3);
        pushButton_setNow->setObjectName(QString::fromUtf8("pushButton_setNow"));

        horizontalLayout_7->addWidget(pushButton_setNow);

        label_25 = new QLabel(frame_3);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        horizontalLayout_7->addWidget(label_25);

        lineEdit_product = new QLineEdit(frame_3);
        lineEdit_product->setObjectName(QString::fromUtf8("lineEdit_product"));

        horizontalLayout_7->addWidget(lineEdit_product);


        verticalLayout_11->addLayout(horizontalLayout_7);


        horizontalLayout_3->addLayout(verticalLayout_11);


        verticalLayout_9->addLayout(horizontalLayout_3);

        check_24bits = new QCheckBox(frame_3);
        check_24bits->setObjectName(QString::fromUtf8("check_24bits"));

        verticalLayout_9->addWidget(check_24bits);

        label_26 = new QLabel(frame_3);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        verticalLayout_9->addWidget(label_26);

        textEdit_Com = new TextEdit(frame_3);
        textEdit_Com->setObjectName(QString::fromUtf8("textEdit_Com"));

        verticalLayout_9->addWidget(textEdit_Com);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 0, -1, -1);
        label = new QLabel(frame_3);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        label_5 = new QLabel(frame_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_2->addWidget(label_5);

        label_3 = new QLabel(frame_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_2->addWidget(label_3);

        label_4 = new QLabel(frame_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_2->addWidget(label_4);

        label_2 = new QLabel(frame_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, -1, -1);
        label_filename = new QLabel(frame_3);
        label_filename->setObjectName(QString::fromUtf8("label_filename"));
        label_filename->setMargin(0);
        label_filename->setIndent(-1);

        verticalLayout->addWidget(label_filename);

        label_sfVersion = new QLabel(frame_3);
        label_sfVersion->setObjectName(QString::fromUtf8("label_sfVersion"));

        verticalLayout->addWidget(label_sfVersion);

        label_soundEngine = new QLabel(frame_3);
        label_soundEngine->setObjectName(QString::fromUtf8("label_soundEngine"));

        verticalLayout->addWidget(label_soundEngine);

        label_romVersion = new QLabel(frame_3);
        label_romVersion->setObjectName(QString::fromUtf8("label_romVersion"));

        verticalLayout->addWidget(label_romVersion);

        label_software = new QLabel(frame_3);
        label_software->setObjectName(QString::fromUtf8("label_software"));

        verticalLayout->addWidget(label_software);


        horizontalLayout_2->addLayout(verticalLayout);


        verticalLayout_9->addLayout(horizontalLayout_2);


        verticalLayout_4->addWidget(frame_3);

        frame_2 = new QFrame(Page_Sf2);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::Panel);
        frame_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        label_7 = new QLabel(frame_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_8->addWidget(label_7);

        label_9 = new QLabel(frame_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        verticalLayout_8->addWidget(label_9);

        label_10 = new QLabel(frame_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout_8->addWidget(label_10);

        label_11 = new QLabel(frame_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        verticalLayout_8->addWidget(label_11);

        label_13 = new QLabel(frame_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout_8->addWidget(label_13);

        label_12 = new QLabel(frame_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout_8->addWidget(label_12);

        label_8 = new QLabel(frame_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        verticalLayout_8->addWidget(label_8);


        horizontalLayout->addLayout(verticalLayout_8);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        label_nbSmpl = new QLabel(frame_2);
        label_nbSmpl->setObjectName(QString::fromUtf8("label_nbSmpl"));

        verticalLayout_7->addWidget(label_nbSmpl);

        label_unusedSmpl = new QLabel(frame_2);
        label_unusedSmpl->setObjectName(QString::fromUtf8("label_unusedSmpl"));

        verticalLayout_7->addWidget(label_unusedSmpl);

        label_nbInst = new QLabel(frame_2);
        label_nbInst->setObjectName(QString::fromUtf8("label_nbInst"));

        verticalLayout_7->addWidget(label_nbInst);

        label_unusedInst = new QLabel(frame_2);
        label_unusedInst->setObjectName(QString::fromUtf8("label_unusedInst"));

        verticalLayout_7->addWidget(label_unusedInst);

        label_nbPrst = new QLabel(frame_2);
        label_nbPrst->setObjectName(QString::fromUtf8("label_nbPrst"));

        verticalLayout_7->addWidget(label_nbPrst);

        label_nbInstGen = new QLabel(frame_2);
        label_nbInstGen->setObjectName(QString::fromUtf8("label_nbInstGen"));

        verticalLayout_7->addWidget(label_nbInstGen);

        label_nbPrstGen = new QLabel(frame_2);
        label_nbPrstGen->setObjectName(QString::fromUtf8("label_nbPrstGen"));

        verticalLayout_7->addWidget(label_nbPrstGen);


        horizontalLayout->addLayout(verticalLayout_7);


        verticalLayout_4->addWidget(frame_2);


        retranslateUi(Page_Sf2);
        QObject::connect(check_24bits, SIGNAL(stateChanged(int)), Page_Sf2, SLOT(set24bits(int)));
        QObject::connect(lineEdit_name, SIGNAL(editingFinished()), Page_Sf2, SLOT(setName()));
        QObject::connect(lineEdit_copyright, SIGNAL(editingFinished()), Page_Sf2, SLOT(setCopyright()));
        QObject::connect(lineEdit_author, SIGNAL(editingFinished()), Page_Sf2, SLOT(setAuthor()));
        QObject::connect(lineEdit_date, SIGNAL(editingFinished()), Page_Sf2, SLOT(setDate()));
        QObject::connect(lineEdit_product, SIGNAL(editingFinished()), Page_Sf2, SLOT(setProduct()));
        QObject::connect(pushButton_setNow, SIGNAL(clicked()), Page_Sf2, SLOT(setNow()));
        QObject::connect(textEdit_Com, SIGNAL(editingFinished()), Page_Sf2, SLOT(setCommentaire()));

        QMetaObject::connectSlotsByName(Page_Sf2);
    } // setupUi

    void retranslateUi(QWidget *Page_Sf2)
    {
        Page_Sf2->setWindowTitle(QApplication::translate("Page_Sf2", "Form", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("Page_Sf2", "Nom", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("Page_Sf2", "Auteur", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("Page_Sf2", "Copyright", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("Page_Sf2", "Date", 0, QApplication::UnicodeUTF8));
        pushButton_setNow->setText(QApplication::translate("Page_Sf2", "Date courante", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("Page_Sf2", "Produit", 0, QApplication::UnicodeUTF8));
        check_24bits->setText(QApplication::translate("Page_Sf2", "Samples 24 bits", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("Page_Sf2", "Commentaires", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Page_Sf2", "Nom du fichier", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Page_Sf2", "Version Soundfont", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Page_Sf2", "Sound engine", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Page_Sf2", "Nom et version ROM", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Page_Sf2", "Logiciel(s) d'\303\251dition", 0, QApplication::UnicodeUTF8));
        label_filename->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_sfVersion->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_soundEngine->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_romVersion->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_software->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Page_Sf2", "Nombre de samples", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Page_Sf2", "Nombre de samples non utilis\303\251s", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Page_Sf2", "Nombre d'instruments", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Page_Sf2", "Nombre d'instruments non utilis\303\251s", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Page_Sf2", "Nombre de presets", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Page_Sf2", "Nombre de param\303\250tres renseign\303\251s pour instruments", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Page_Sf2", "Nombre de param\303\250tres renseign\303\251s pour presets", 0, QApplication::UnicodeUTF8));
        label_nbSmpl->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_unusedSmpl->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_nbInst->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_unusedInst->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_nbPrst->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_nbInstGen->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_nbPrstGen->setText(QApplication::translate("Page_Sf2", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Page_Sf2: public Ui_Page_Sf2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_SF2_H
